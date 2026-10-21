"""Check optional Rifts advancement with actual NPC, body, stat and health code."""

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


class RiftsAdvancementTests(unittest.TestCase):
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

    def test_stat_pairing_and_hp_bookkeeping(self):
        self.assertTrue(COMPILER.is_file(), f"Build lpc-rs first: {COMPILER}")
        with tempfile.TemporaryDirectory(prefix="lima-rifts-advancement-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copytree(FIXTURES, library / "port_tests/rifts_advancement")
            combat = library / "include/combat_modules.h"
            baseline_combat = combat.read_text(encoding="utf-8")
            selector = "#define ADVANCEMENT_STYLE  ADVANCEMENT_SIMPLE"
            self.assertEqual(baseline_combat.count(selector), 1)
            config = library / "include/config.h"
            baseline_config = config.read_text(encoding="utf-8")
            stats_selector = "#define BODY_STATS BODY_STATS_LIMA"
            self.assertEqual(baseline_config.count(stats_selector), 1)
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
            combat.write_text(baseline_combat.replace(
                selector, "#define ADVANCEMENT_STYLE  ADVANCEMENT_RIFTS"
            ), encoding="utf-8")
            user = library / "secure/user.c"
            master = library / "secure/master.c"
            original_user = user.read_bytes()
            original_master = master.read_bytes()
            for variant in ("LIMA", "RIFTS"):
                with self.subTest(stats=variant):
                    config.write_text(baseline_config.replace(
                        stats_selector, "#define BODY_STATS BODY_STATS_" + variant
                    ), encoding="utf-8")
                    user.write_bytes(original_user)
                    master.write_bytes(original_master)
                    targets = [
                        "std/adversary.c", "std/body.c", "std/race/human.c",
                        "domains/std/trainer.c", "domains/std/monster/troll.c",
                        "secure/user.c", "secure/master.c",
                        "port_tests/rifts_advancement/actor.c",
                        "port_tests/rifts_advancement/check.c",
                    ]
                    if variant == "RIFTS":
                        targets.append("std/race/dragon.c")
                    for source in targets:
                        self.assertEqual(self.run_compiler(
                            temporary, profile, library / source
                        ), "")
                    shutil.copy2(FIXTURES / "user.c", user)
                    self.assertEqual(self.run_compiler(temporary, profile, user), "")
                    shutil.copy2(FIXTURES / "check.c", master)
                    output = self.run_compiler(temporary, profile, master, execute=True)
                    lines = output.splitlines()
                    self.assertTrue(lines, output)
                    self.assertEqual(lines[-1], "RIFTS_ADVANCEMENT_TOTAL: 17", output)
                    labels = lines[:-1]
                    self.assertEqual(len(labels), 17, output)
                    self.assertEqual(len(set(labels)), 17, output)
                    self.assertTrue(all(
                        label.startswith("RIFTS_ADVANCEMENT_OK: ") for label in labels
                    ), output)


if __name__ == "__main__":
    unittest.main(verbosity=2)
