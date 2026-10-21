"""Exercise NPC vaporization and real worn/unworn inventory without a listener."""

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


class VaporizeTests(unittest.TestCase):
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

    def test_npc_and_inventory_vaporization(self):
        self.assertTrue(COMPILER.is_file(), f"Build lpc-rs first: {COMPILER}")
        with tempfile.TemporaryDirectory(prefix="lima-vaporize-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copytree(FIXTURES, library / "port_tests/vaporize")
            config = library / "include/combat_modules.h"
            baseline = config.read_text(encoding="utf-8")
            selector = "#define DEATH_STYLE        DEATH_CORPSES"
            self.assertEqual(baseline.count(selector), 1)
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_NULL_MODE=distinct\n", settings)
            self.assertIn("LPC_CALL_OUT_PRESERVE_PLAYER=true\n", settings)
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile = temporary / "lpc-rs.env"
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ), encoding="utf-8")
            for source in ("std/adversary.c", "std/body.c",
                           "std/race/human.c", "domains/std/trainer.c"):
                self.assertEqual(self.run_compiler(temporary, profile, library / source), "")
            config.write_text(baseline.replace(
                selector, "#define DEATH_STYLE        DEATH_VAPORIZE"
            ), encoding="utf-8")
            for source in (
                "std/adversary.c", "std/body.c", "std/race/human.c",
                "domains/std/trainer.c", "domains/std/monster/troll.c",
                "daemons/loot_d.c", "std/armour.c", "secure/master.c",
                "port_tests/vaporize/actor.c", "port_tests/vaporize/room.c",
                "port_tests/vaporize/armour.c", "port_tests/vaporize/item.c",
                "port_tests/vaporize/check.c",
            ):
                self.assertEqual(self.run_compiler(temporary, profile, library / source), "")
            shutil.copy2(FIXTURES / "check.c", library / "secure/master.c")
            output = self.run_compiler(
                temporary, profile, library / "secure/master.c", execute=True
            )
            self.assertNotIn("VAPORIZE_FAIL:", output)
            self.assertTrue(output.rstrip().endswith("VAPORIZE_TOTAL: 22"), output)
            labels = [line for line in output.splitlines() if line.startswith("VAPORIZE_OK: ")]
            self.assertEqual(len(labels), 22, output)
            self.assertEqual(len(set(labels)), 22, output)


if __name__ == "__main__":
    unittest.main(verbosity=2)
