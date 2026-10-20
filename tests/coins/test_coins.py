"""Check denomination conversion and value conservation through real get/drop code."""

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


class CoinTests(unittest.TestCase):
    def test_pile_units_and_value_conservation(self):
        for binary in (COMPILER, DRIVER):
            self.assertTrue(binary.is_file(), f"Build lpc-rs first: {binary}")
        with tempfile.TemporaryDirectory(prefix="lima-coins-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copytree(FIXTURES, library / "port_tests/coins")
            shutil.copy2(FIXTURES / "master.c", library / "secure/master.c")
            for name in ("data/daemons",):
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
                "std/coins.c", "cmds/verbs/drop.c", "cmds/verbs/get.c",
                "daemons/money_d.c", "std/body.c", "daemons/loot_d.c",
                "port_tests/coins/actor.c", "port_tests/coins/refusing_room.c",
                "port_tests/coins/refusing_pile.c", "secure/master.c",
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
            for marker in ("COINS_ERROR:", "COINS_FAIL:", "Apply failed", "Transaction failed"):
                self.assertNotIn(marker, output)
            labels = [line for line in output.splitlines() if line.startswith("COINS_OK: ")]
            self.assertEqual(len(labels), 55, output)
            self.assertEqual(len(set(labels)), 55, output)
            self.assertIn("COINS_TOTAL: 55", output)



if __name__ == "__main__":
    unittest.main(verbosity=2)
