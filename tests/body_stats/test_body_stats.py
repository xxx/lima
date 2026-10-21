"""Check the shipped body-stat selectors using actual menu and race sources."""

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


class BodyStatTests(unittest.TestCase):
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

    def test_body_stat_selections(self):
        self.assertTrue(COMPILER.is_file(), f"Build lpc-rs first: {COMPILER}")
        with tempfile.TemporaryDirectory(prefix="lima-body-stats-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            original_user = (library / "secure/user.c").read_bytes()
            baseline = (library / "include/config.h").read_text(encoding="utf-8")
            selector = "#define BODY_STATS BODY_STATS_LIMA"
            self.assertEqual(baseline.count(selector), 1)
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_NULL_MODE=distinct\n", settings)
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile = temporary / "lpc-rs.env"
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ), encoding="utf-8")
            shutil.copy2(FIXTURES / "check.c", library / "secure/master.c")
            for variant, expected in (("LIMA", 124), ("DND", 56), ("RIFTS", 60)):
                with self.subTest(selector=variant):
                    (library / "include/config.h").write_text(baseline.replace(
                        selector, "#define BODY_STATS BODY_STATS_" + variant
                    ), encoding="utf-8")
                    (library / "secure/user.c").write_bytes(original_user)
                    for source in ("std/adversary.c", "std/body.c",
                                   "obj/usermenu/usermenu.c", "secure/user.c"):
                        self.assertEqual(self.run_compiler(
                            temporary, profile, library / source
                        ), "")
                    shutil.copy2(FIXTURES / "user.c", library / "secure/user.c")
                    for source in ("secure/user.c", "secure/master.c"):
                        self.assertEqual(self.run_compiler(
                            temporary, profile, library / source
                        ), "")
                    output = self.run_compiler(
                        temporary, profile, library / "secure/master.c", execute=True
                    )
                    lines = output.splitlines()
                    self.assertTrue(lines, output)
                    self.assertEqual(lines[-1], f"BODY_STATS_TOTAL: {expected}", output)
                    labels = lines[:-1]
                    self.assertEqual(len(labels), expected, output)
                    self.assertEqual(len(set(labels)), expected, output)
                    self.assertTrue(all(
                        label.startswith("BODY_STATS_OK: ") for label in labels
                    ), output)


if __name__ == "__main__":
    unittest.main(verbosity=2)
