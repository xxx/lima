"""Exercise simple damage and preserve the selected typed combat path."""

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


class SimpleBlowTests(unittest.TestCase):
    def run_compiler(self, temporary, profile, target, *, execute=False):
        command = [str(COMPILER), "-c", str(profile)]
        if not execute:
            command.append("--check")
        result = subprocess.run(
            [*command, str(target)], cwd=temporary,
            env={"PATH": os.defpath, "TOKIO_WORKER_THREADS": "2",
                 "RUST_MIN_STACK": "16777216"},
            capture_output=True, text=True, timeout=40, preexec_fn=process_limits,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(result.stderr, "", result.stderr)
        return result.stdout

    def test_combat_payload_selections(self):
        self.assertTrue(COMPILER.is_file(), f"Build lpc-rs first: {COMPILER}")
        with tempfile.TemporaryDirectory(prefix="lima-simple-blows-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copytree(FIXTURES, library / "port_tests/simple_blows")
            config = library / "include/combat_modules.h"
            baseline = config.read_text(encoding="utf-8")
            selector = "#define BLOW_STYLE         BLOW_TYPES"
            self.assertEqual(baseline.count(selector), 1)
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_NULL_MODE=distinct\n", settings)
            self.assertIn("LPC_CALL_OUT_PRESERVE_PLAYER=true\n", settings)
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile = temporary / "lpc-rs.env"
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ), encoding="utf-8")
            for variant, expected in (("TYPES", 33), ("SIMPLE", 32)):
                with self.subTest(selector=variant):
                    config.write_text(baseline.replace(
                        selector, "#define BLOW_STYLE         BLOW_" + variant
                    ), encoding="utf-8")
                    for source in (
                        "std/adversary.c", "std/body.c", "std/race/human.c",
                        "domains/std/trainer.c", "std/armour.c", "std/weapon.c",
                        "domains/std/monster/test_dummy.c",
                        "domains/std/spell/transient/shield.c",
                        "port_tests/simple_blows/armour.c",
                        "port_tests/simple_blows/weapon.c",
                        "port_tests/simple_blows/check.c",
                    ):
                        self.assertEqual(self.run_compiler(
                            temporary, profile, library / source
                        ), "")
                    shutil.copy2(FIXTURES / "check.c", library / "secure/master.c")
                    output = self.run_compiler(
                        temporary, profile, library / "secure/master.c", execute=True
                    )
                    lines = output.splitlines()
                    self.assertTrue(lines, output)
                    self.assertEqual(lines[-1], f"SIMPLE_BLOWS_TOTAL: {expected}", output)
                    labels = lines[:-1]
                    self.assertEqual(len(labels), expected, output)
                    self.assertEqual(len(set(labels)), expected, output)
                    self.assertTrue(all(
                        label.startswith("SIMPLE_BLOWS_OK: ") for label in labels
                    ), output)


if __name__ == "__main__":
    unittest.main(verbosity=2)
