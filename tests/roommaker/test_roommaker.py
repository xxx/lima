"""Exercise the contributed room maker and construct its generated rooms."""

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


class RoomMakerTests(unittest.TestCase):
    def run_process(self, args, directory):
        result = subprocess.run(
            args, cwd=directory,
            env={"PATH": os.defpath, "TOKIO_WORKER_THREADS": "2",
                 "RUST_MIN_STACK": "16777216"},
            capture_output=True, text=True, timeout=35, preexec_fn=process_limits,
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        return result.stdout + result.stderr

    def check_source(self, profile, library, target, directory):
        output = self.run_process(
            [str(COMPILER), "-c", str(profile), "--check", str(library / target)],
            directory,
        )
        self.assertEqual(output, "")

    def run_driver(self, profile, directory, count):
        output = self.run_process([str(DRIVER), "-e", str(profile)], directory)
        self.assertNotIn("ROOM_ERROR:", output)
        self.assertNotIn("ROOM_FAIL:", output)
        self.assertNotIn("Transaction failed", output)
        labels = [line for line in output.splitlines() if line.startswith("ROOM_OK: ")]
        self.assertEqual(len(labels), count, output)
        self.assertEqual(len(set(labels)), count, output)
        self.assertIn(f"ROOM_TOTAL: {count}", output)

    def test_menu_editing_and_generated_rooms(self):
        for binary in (COMPILER, DRIVER):
            self.assertTrue(binary.is_file(), f"Build lpc-rs first: {binary}")
        with tempfile.TemporaryDirectory(prefix="lima-roommaker-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            shutil.copytree(FIXTURES, library / "port_tests/roommaker")
            shutil.copy2(FIXTURES / "generate.c", library / "secure/master.c")
            (library / "generated").mkdir()
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            self.assertIn("LPC_LIB_DIR=lib\n", settings)
            profile = temporary / "lpc-rs.env"
            profile.write_text(settings.replace(
                "LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n"
            ) + "LPC_BIND_ADDRESS=127.0.0.1\nLPC_PORT=0\n", encoding="utf-8")
            for target in (
                "contrib/roommaker/makeroom.c", "contrib/roommaker/buildmenu.c",
                "std/menu_old.c", "trans/obj/edit_ob.c",
                "std/indoor_room.c", "std/outdoor_room.c",
                "port_tests/roommaker/user.c", "secure/master.c",
            ):
                self.check_source(profile, library, target, temporary)
            self.run_driver(profile, temporary, 46)
            generated = ("default", "indoor", "outdoor", "reset", "composed", "other")
            self.assertEqual({path.stem for path in (library / "generated").iterdir()}, set(generated))
            for name in generated:
                self.check_source(profile, library, f"generated/{name}.c", temporary)
            text = (library / "generated/composed.c").read_text(encoding="utf-8")
            self.assertEqual(text.count("#include <mudlib.h>"), 1)
            self.assertEqual(text.count("void setup()"), 1)
            self.assertEqual(text.count("set_light("), 1)
            self.assertNotIn("set_light(3)", text)
            self.assertIn("set_light(4)", text)
            self.assertFalse((library / "generated/absent/failure.c").exists())
            shutil.copy2(FIXTURES / "check.c", library / "secure/master.c")
            self.check_source(profile, library, "secure/master.c", temporary)
            self.run_driver(profile, temporary, 17)


if __name__ == "__main__":
    unittest.main(verbosity=2)
