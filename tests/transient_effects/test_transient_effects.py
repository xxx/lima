"""Exercise contributed effects, movement and elapsed callbacks in a real driver."""

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


class TransientEffectTests(unittest.TestCase):
    def test_effects_and_elapsed_callbacks(self):
        for binary in (COMPILER, DRIVER):
            self.assertTrue(binary.is_file(), f"Build lpc-rs first: {binary}")
        with tempfile.TemporaryDirectory(prefix="lima-transient-effects-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            fixtures = library / "port_tests/transient_effects"
            shutil.copytree(FIXTURES, fixtures)
            shutil.copy2(FIXTURES / "master.c", library / "secure/master.c")
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile = temporary / "lpc-rs.env"
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ) + "LPC_BIND_ADDRESS=127.0.0.1\nLPC_PORT=0\n", encoding="utf-8")
            env = {"PATH": os.defpath, "TOKIO_WORKER_THREADS": "2",
                   "RUST_MIN_STACK": "16777216"}
            targets = (
                "contrib/transient_effect/transient_effect.c",
                "contrib/transient_effect/drunk.c",
                "std/transient.c", "std/transient/poison.c", "std/transient/speed.c",
                "port_tests/transient_effects/holder.c",
                "port_tests/transient_effects/probe.c", "secure/master.c",
            )
            for target in targets:
                with self.subTest(source=target):
                    result = subprocess.run(
                        [str(COMPILER), "-c", str(profile), "--check", str(library / target)],
                        cwd=temporary, env=env, capture_output=True, text=True,
                        timeout=40, preexec_fn=process_limits,
                    )
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    self.assertEqual(result.stderr, "")
            result = subprocess.run(
                [str(DRIVER), "-e", str(profile)], cwd=temporary, env=env,
                capture_output=True, text=True, timeout=35, preexec_fn=process_limits,
            )
            output = result.stdout + result.stderr
            self.assertEqual(result.returncode, 0, output)
            self.assertNotIn("EFFECT_ERROR:", output)
            self.assertNotIn("EFFECT_FAIL:", output)
            self.assertNotIn("Transaction failed", output)
            checks = [line for line in output.splitlines() if line.startswith("EFFECT_OK:")]
            self.assertEqual(len(checks), 36, output)
            self.assertEqual(len(set(checks)), 36, output)
            self.assertIn("EFFECT_TOTAL: 36", output)


if __name__ == "__main__":
    unittest.main(verbosity=2)
