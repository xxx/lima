"""Exercise optional wound health and its real fifteen-second callback."""

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
EXPECTED_CHECKS = 46


def process_limits():
    resource.setrlimit(resource.RLIMIT_AS, (4 * 1024**3, 4 * 1024**3))


class WoundHealthTests(unittest.TestCase):
    def test_damage_healing_and_elapsed_timer(self):
        for binary in (COMPILER, DRIVER):
            self.assertTrue(binary.is_file(), f"Build lpc-rs first: {binary}")
        with tempfile.TemporaryDirectory(prefix="lima-wounds-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copytree(FIXTURES, library / "port_tests/wounds")
            shutil.copy2(FIXTURES / "master.c", library / "secure/master.c")
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_NULL_MODE=distinct\n", settings)
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile = temporary / "lpc-rs.env"
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ) + "LPC_BIND_ADDRESS=127.0.0.1\nLPC_PORT=0\n", encoding="utf-8")
            env = {"PATH": os.defpath, "TOKIO_WORKER_THREADS": "2",
                   "RUST_MIN_STACK": "16777216"}

            def compile_source(source):
                result = subprocess.run(
                    [str(COMPILER), "-c", str(profile), "--check", str(library / source)],
                    cwd=temporary, env=env, capture_output=True, text=True,
                    timeout=40, preexec_fn=process_limits,
                )
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(result.stdout + result.stderr, "")

            for source in ("std/adversary.c", "std/body.c", "std/race/human.c",
                           "domains/std/trainer.c"):
                compile_source(source)
            config = library / "include/combat_modules.h"
            baseline = config.read_text(encoding="utf-8")
            selector = "#define HEALTH_STYLE       HEALTH_LIMBS"
            self.assertEqual(baseline.count(selector), 1)
            config.write_text(baseline.replace(
                selector, "#define HEALTH_STYLE       HEALTH_WOUNDS"
            ), encoding="utf-8")
            for source in ("std/adversary.c", "std/body.c", "std/race/human.c",
                           "domains/std/trainer.c", "cmds/verbs/diagnose.c",
                           "obj/usermenu/usermenu.c", "secure/user.c"):
                compile_source(source)
            shutil.copy2(FIXTURES / "user.c", library / "secure/user.c")
            for source in ("secure/user.c", "port_tests/wounds/actor.c", "secure/master.c"):
                compile_source(source)
            result = subprocess.run(
                [str(DRIVER), "-e", str(profile)], cwd=temporary, env=env,
                capture_output=True, text=True, timeout=45, preexec_fn=process_limits,
            )
            output = result.stdout + result.stderr
            self.assertEqual(result.returncode, 0, output)
            for marker in ("WOUNDS_ERROR:", "WOUNDS_FAIL:", "Apply failed", "Transaction failed"):
                self.assertNotIn(marker, output)
            labels = [line for line in output.splitlines() if line.startswith("WOUNDS_OK: ")]
            self.assertEqual(len(labels), EXPECTED_CHECKS, output)
            self.assertEqual(len(set(labels)), EXPECTED_CHECKS, output)
            self.assertIn(f"WOUNDS_TOTAL: {EXPECTED_CHECKS}", output)


if __name__ == "__main__":
    unittest.main(verbosity=2)
