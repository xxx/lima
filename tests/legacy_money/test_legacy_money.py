"""Exercise integer denomination-count money without selecting the optional provider."""

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


class LegacyMoneyTests(unittest.TestCase):
    def test_denomination_counts_and_wallet_credit(self):
        for binary in (COMPILER, DRIVER):
            self.assertTrue(binary.is_file(), f"Build lpc-rs first: {binary}")
        with tempfile.TemporaryDirectory(prefix="lima-legacy-money-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copytree(FIXTURES, library / "port_tests/legacy_money")
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
                "std/money.c", "std/coins.c", "cmds/verbs/get.c", "cmds/verbs/drop.c",
                "daemons/money_d.c", "std/body.c",
                "port_tests/legacy_money/actor.c", "secure/master.c",
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
            for marker in ("LEGACY_MONEY_ERROR:", "LEGACY_MONEY_FAIL:", "Apply failed", "Transaction failed"):
                self.assertNotIn(marker, output)
            labels = [line for line in output.splitlines() if line.startswith("LEGACY_MONEY_OK: ")]
            self.assertEqual(len(labels), 39, output)
            self.assertEqual(len(set(labels)), 39, output)
            self.assertIn("LEGACY_MONEY_TOTAL: 39", output)



if __name__ == "__main__":
    unittest.main(verbosity=2)
