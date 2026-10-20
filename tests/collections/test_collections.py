"""Exercise the shipped queue and stack helpers in an isolated Lima library."""

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


def process_limits():
    resource.setrlimit(resource.RLIMIT_AS, (4 * 1024**3, 4 * 1024**3))


class CollectionTests(unittest.TestCase):
    def run_compiler(self, profile, target, *, execute=False):
        env = {
            key: value for key, value in os.environ.items()
            if not key.startswith("LPC_") and key not in {
                "LIB_DIR", "MASTER_OBJECT", "SIMUL_EFUN_FILE", "AUTO_INCLUDE_FILE",
                "AUTO_INHERIT_FILE", "SYSTEM_INCLUDE_DIRS", "MAX_EXECUTION_TIME",
                "NULL_MODE", "CALL_OUT_PRESERVE_PLAYER",
            }
        }
        env.update(TOKIO_WORKER_THREADS="2", RUST_MIN_STACK="16777216")
        command = [str(COMPILER), "-c", str(profile)]
        if not execute:
            command.append("--check")
        result = subprocess.run(
            [*command, str(target)], cwd=ROOT, env=env, capture_output=True,
            text=True, timeout=40, preexec_fn=process_limits,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(result.stderr, "", result.stderr)
        return result.stdout

    def test_queue_and_stack(self):
        self.assertTrue(COMPILER.is_file(), f"Build lpc-rs first: {COMPILER}")
        with tempfile.TemporaryDirectory(prefix="lima-collections-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copy2(FIXTURES / "master.c", library / "secure/master.c")
            profile = temporary / "lpc-rs.env"
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ), encoding="utf-8")
            for name in ("last_queue.c", "stack.c"):
                with self.subTest(source=name):
                    self.run_compiler(profile, library / "contrib" / name)
            self.run_compiler(profile, library / "secure/master.c")
            output = self.run_compiler(
                profile, library / "secure/master.c", execute=True
            )
            lines = output.splitlines()
            self.assertEqual(lines[-1], "COLLECTION_TOTAL: 37", output)
            checks = lines[:-1]
            self.assertEqual(len(checks), 37, output)
            self.assertEqual(len(set(checks)), 37, output)
            self.assertTrue(all(
                line.startswith("COLLECTION_OK: ") for line in checks
            ), output)


if __name__ == "__main__":
    unittest.main(verbosity=2)
