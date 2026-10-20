"""Exercise the shipped tar writer and elapsed continuations in a real driver."""

import io
import os
from pathlib import Path
import resource
import shutil
import subprocess
import tarfile
import tempfile
import time
import unittest

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = Path(__file__).resolve().parent / "fixtures"
COMPILER = Path(os.environ.get(
    "LPC_RS_COMPILER", ROOT.parent / "lpc-rs/target/release/lpc-rs-lpcc"
)).resolve()
DRIVER = Path(os.environ.get(
    "LPC_RS_DRIVER", ROOT.parent / "lpc-rs/target/release/lpc-rs-driver"
)).resolve()
LARGE = bytes(range(256)) * 65536 + (b"after-16MiB\n" + bytes(range(256)) * 2)[:513]
TREE_FILES = {
    "empty.bin": b"",
    "one.bin": b"A",
    "bytes511.bin": bytes(range(256)) + bytes(range(255)),
    "bytes512.bin": bytes(range(256)) * 2,
    "bytes513.bin": bytes(range(256)) * 2 + b"Z",
    "nested/é.bin": b"\x00\xffA\n",
    ".hidden": b"hidden\x00",
    **{f"many/item{i:02}.bin": bytes([i]) for i in range(14)},
}
TREE_DIRS = {"", "empty", "empty/deeper", "many", "nested"}


def process_limits():
    resource.setrlimit(resource.RLIMIT_AS, (4 * 1024**3, 4 * 1024**3))


def archive_size(files, directories=()):
    return 1024 + 512 * len(directories) + sum(
        512 + (len(data) + 511) // 512 * 512 for data in files.values()
    )


class TarTests(unittest.TestCase):
    def verify_archive(self, path, files, directories, started, finished):
        raw = path.read_bytes()
        self.assertEqual(len(raw), archive_size(files, directories))
        self.assertEqual(raw[-1024:], bytes(1024))
        with tarfile.open(fileobj=io.BytesIO(raw), mode="r:") as archive:
            members = archive.getmembers()
            self.assertEqual(len(members), len(files) + len(directories))
            self.assertEqual(set(archive.getnames()), set(files) | set(directories))
            for member in members:
                with self.subTest(archive=path.name, member=member.name):
                    self.assertEqual((member.uid, member.gid), (1701, 2261))
                    self.assertEqual((member.uname, member.gname), ("zell", "alyx"))
                    self.assertEqual(member.mode, 0o40755)
                    self.assertLessEqual(started, member.mtime)
                    self.assertLessEqual(member.mtime, finished)
                    header = raw[member.offset:member.offset + 512]
                    self.assertEqual(header[257:265], b"ustar  \x00")
                    if member.name in directories:
                        self.assertTrue(member.isdir())
                        self.assertEqual(member.size, 0)
                    else:
                        self.assertTrue(member.isfile())
                        expected = files[member.name]
                        self.assertEqual(member.size, len(expected))
                        self.assertEqual(archive.extractfile(member).read(), expected)
                        end = member.offset_data + member.size
                        padded = member.offset_data + (member.size + 511) // 512 * 512
                        self.assertEqual(raw[end:padded], bytes(padded - end))
        listing = subprocess.run(
            ["tar", "--quoting-style=literal", "-tf", str(path)],
            env={"PATH": os.defpath, "LC_ALL": "C.UTF-8"},
            capture_output=True, text=True, timeout=10,
        )
        self.assertEqual(listing.returncode, 0, listing.stderr)
        self.assertEqual(listing.stderr, "")
        self.assertEqual(set(listing.stdout.splitlines()),
                         set(files) | {name + "/" for name in directories})
        if files:
            name = next(iter(files))
            result = subprocess.run(
                ["tar", "-xOf", str(path), name], capture_output=True, timeout=10,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stderr, b"")
            self.assertEqual(result.stdout, files[name])

    def test_archives_and_continuations(self):
        for binary in (COMPILER, DRIVER):
            self.assertTrue(binary.is_file(), f"Build lpc-rs first: {binary}")
        self.assertIsNotNone(shutil.which("tar"), "GNU tar is required")
        with tempfile.TemporaryDirectory(prefix="lima-tar-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            for name, data in TREE_FILES.items():
                path = library / "input/tree" / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(data)
            for name in TREE_DIRS:
                (library / "input/tree" / name).mkdir(parents=True, exist_ok=True)
            (library / "input/tree/inside.tar").write_bytes(b"obsolete archive contents")
            (library / "input/emptydir").mkdir()
            (library / "input/large.bin").write_bytes(LARGE)
            (library / "output").mkdir()
            (library / "output/rejected.tar").write_bytes(b"keep")
            for name in ("n" * 100, "n" * 101, "é" * 50, "é" * 51):
                (library / name).write_bytes(b"\x00\xffX")
            (library / ("d" * 100)).mkdir()
            with (library / "oversize.bin").open("wb") as file:
                file.truncate(8589934592)
            tree_files = {"input/tree/" + name: data for name, data in TREE_FILES.items()}
            tree_dirs = {("input/tree/" + name).rstrip("/") for name in TREE_DIRS}
            master = (FIXTURES / "master.c").read_text(encoding="utf-8")
            (library / "secure/master.c").write_text(
                f"#define TREE_ARCHIVE_SIZE {archive_size(tree_files, tree_dirs)}\n"
                f"#define LARGE_ARCHIVE_SIZE {archive_size({'input/large.bin': LARGE})}\n"
                + master, encoding="utf-8",
            )
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile = temporary / "lpc-rs.env"
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ) + "LPC_BIND_ADDRESS=127.0.0.1\nLPC_PORT=0\n", encoding="utf-8")
            env = {"PATH": os.defpath, "TOKIO_WORKER_THREADS": "2",
                   "RUST_MIN_STACK": "16777216"}
            for target in ("contrib/tar.c", "secure/master.c"):
                result = subprocess.run(
                    [str(COMPILER), "-c", str(profile), "--check", str(library / target)],
                    cwd=temporary, env=env, capture_output=True, text=True,
                    timeout=40, preexec_fn=process_limits,
                )
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(result.stderr, "")
            started = int(time.time())
            result = subprocess.run(
                [str(DRIVER), "-e", str(profile)], cwd=temporary, env=env,
                capture_output=True, text=True, timeout=50, preexec_fn=process_limits,
            )
            finished = int(time.time())
            output = result.stdout + result.stderr
            self.assertEqual(result.returncode, 0, output)
            self.assertNotIn("TAR_ERROR:", output)
            self.assertNotIn("TAR_FAIL:", output)
            checks = [line for line in output.splitlines() if line.startswith("TAR_OK:")]
            self.assertEqual(len(checks), 21, output)
            self.assertEqual(len(set(checks)), 21, output)
            self.assertIn("TAR_TOTAL: 21", output)
            expected = {
                "input/tree/inside.tar": (tree_files, tree_dirs),
                "output/directory.tar": ({}, {"input/emptydir"}),
                "output/large.tar": ({"input/large.bin": LARGE}, set()),
                "output/empty.tar": ({"input/tree/empty.bin": b""}, set()),
                "output/ascii.tar": ({"n" * 100: b"\x00\xffX"}, set()),
                "output/unicode.tar": ({"é" * 50: b"\x00\xffX"}, set()),
            }
            for name, (files, directories) in expected.items():
                with self.subTest(archive=name):
                    self.verify_archive(library / name, files, directories, started, finished)
            self.assertEqual((library / "output/rejected.tar").read_bytes(), b"keep")


if __name__ == "__main__":
    unittest.main(verbosity=2)
