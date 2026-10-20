"""Exercise the web-editor backend through local CGI forms and actual file APIs."""

import os
from pathlib import Path
import resource
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = Path(__file__).resolve().parent / "fixtures"
COMPILER = Path(os.environ.get(
    "LPC_RS_COMPILER", ROOT.parent / "lpc-rs/target/release/lpc-rs-lpcc"
)).resolve()
DRIVER = Path(os.environ.get(
    "LPC_RS_DRIVER", ROOT.parent / "lpc-rs/target/release/lpc-rs-driver"
)).resolve()


def process_limits():
    resource.setrlimit(resource.RLIMIT_AS, (4 * 1024**3, 4 * 1024**3))


class WebEditorTests(unittest.TestCase):
    def test_forms_and_committed_files(self):
        for binary in (COMPILER, DRIVER):
            self.assertTrue(binary.is_file(), f"Build lpc-rs first: {binary}")
        with tempfile.TemporaryDirectory(prefix="lima-web-editor-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copy2(FIXTURES / "master.c", library / "secure/master.c")
            for name in ("data/links/w", "wiz/webaudit", "data/secure/daemons"):
                (library / name).mkdir(parents=True, exist_ok=True)
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile = temporary / "lpc-rs.env"
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ) + "LPC_BIND_ADDRESS=127.0.0.1\nLPC_PORT=0\n", encoding="utf-8")
            env = {"PATH": os.defpath, "TOKIO_WORKER_THREADS": "2",
                   "RUST_MIN_STACK": "16777216"}
            for target in (
                "obj/secure/webed.c", "secure/cgi/webed.c",
                "secure/daemons/user_d.c", "secure/daemons/secure_d.c",
                "secure/modules/m_access.c", "secure/simul_efun.c", "secure/master.c",
            ):
                result = subprocess.run(
                    [str(COMPILER), "-c", str(profile), "--check", str(library / target)],
                    cwd=temporary, env=env, capture_output=True, text=True,
                    timeout=35, preexec_fn=process_limits,
                )
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(result.stdout + result.stderr, "")
            result = subprocess.run(
                [str(DRIVER), "-e", str(profile)], cwd=temporary, env=env,
                capture_output=True, text=True, timeout=35, preexec_fn=process_limits,
            )
            output = result.stdout + result.stderr
            self.assertEqual(result.returncode, 0, output)
            for marker in ("WEBED_ERROR:", "WEBED_FAIL:", "Apply failed", "Transaction failed"):
                self.assertNotIn(marker, output)
            labels = [line for line in output.splitlines() if line.startswith("WEBED_OK: ")]
            self.assertEqual(len(labels), 20, output)
            self.assertEqual(len(set(labels)), 20, output)
            self.assertIn("WEBED_TOTAL: 20", output)
            files = library / "wiz/webaudit"
            expected = {
                "saved.txt": b"replacement\n",
                "empty-saved.txt": b"",
                "full.txt": b"freshly reloaded\n",
                "unicode.txt": "café <window> & λ\n".encode("utf-8"),
            }
            for name, contents in expected.items():
                with self.subTest(file=name):
                    self.assertEqual((files / name).read_bytes(), contents)
            for name in ("absent/file.txt", "refused.txt", "new.txt"):
                self.assertFalse((files / name).exists(), name)


if __name__ == "__main__":
    unittest.main(verbosity=2)
