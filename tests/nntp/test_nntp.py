"""Qualify the optional NNTP client using only a recording transport and the CLI."""

from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import resource
import shutil
import subprocess
import tempfile
import time
import unittest

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = Path(__file__).resolve().parent / "fixtures"
COMPILER = Path(os.environ.get(
    "LPC_RS_COMPILER", ROOT.parent / "lpc-rs/target/release/lpc-rs-lpcc"
)).resolve()
TEST_PATH = "/port_tests/nntp"


def process_limits():
    resource.setrlimit(resource.RLIMIT_AS, (4 * 1024**3, 4 * 1024**3))


def article_wire(text):
    lines = text.replace("\r\n", "\n").split("\n") if text else []
    if lines and lines[-1] == "":
        lines.pop()
    return "".join(("." if line.startswith(".") else "") + line + "\r\n"
                   for line in lines) + ".\r\n"


def write_data(directory):
    directory.mkdir()
    large = "Subject: recording\n\n" + "".join(
        f".line {index:04} " + "é中🐉" * 25 + "\n" for index in range(1100)
    ) + "..last\n.\nend"
    small = "Subject: small\r\n\r\n.dot\r\n..\r\n.\r\nend"
    endings = ["", "\n", "\n\n", ".", ".\n", "x\r\n\r\n", "x\n"]
    articles = [("large", large), ("small", small)]
    articles += [(f"ending{index}", text) for index, text in enumerate(endings)]
    for name, text in articles:
        (directory / f"{name}.txt").write_text(text, encoding="utf-8", newline="")
        (directory / f"{name}.expected").write_text(
            article_wire(text), encoding="utf-8", newline=""
        )
    post = "POST\r\n" + article_wire(large)
    have = post + "IHAVE <b@example.invalid>\r\n" + article_wire(small)
    for name, text in (
        ("post", post), ("post-ihave", post + "IHAVE <b@example.invalid>\r\n"),
        ("have", have), ("have-help", have + "HELP\r\n"),
    ):
        (directory / f"{name}.expected").write_text(text, encoding="utf-8", newline="")
    (directory / "long-line.txt").write_text("x" * 70000 + "\r\n.\r\n", encoding="utf-8", newline="")


def write_dates(target):
    stamps = {
        -2208988801, -2208988800, -1, 0, 1, 86399, 86400,
        951782399, 951782400, 951868799, 951868800,
        4107542399, 4107542400, 13569465600,
    }
    for year in (1900, 1969, 1970, 2000, 2024, 2100, 2400):
        for month in (1, 2, 3, 7, 12):
            stamps.add(int(datetime(year, month, 1, 12, 34, 56, tzinfo=timezone.utc).timestamp()))
    for instant in (
        "2024-03-10T09:59:59", "2024-03-10T10:00:00",
        "2024-11-03T08:59:59", "2024-11-03T09:00:00",
    ):
        stamps.add(int(datetime.fromisoformat(instant).replace(tzinfo=timezone.utc).timestamp()))
    lines = [f'inherit "{TEST_PATH}/check";', "void create()", "{", "   ready();"]
    for stamp in sorted(stamps):
        expected = datetime.fromtimestamp(stamp, timezone.utc).strftime("%y%m%d %H%M%S")
        lines.append(f'   verify(client->dt_stamp({stamp}) == "{expected}", "UTC epoch {stamp}");')
    target.write_text("\n".join(lines + ["   total();", "}"]) + "\n", encoding="utf-8")
    return len(stamps)


class NntpTests(unittest.TestCase):
    def test_offline_client(self):
        self.assertTrue(COMPILER.is_file(), f"Build lpc-rs first: {COMPILER}")
        logs = os.environ.get("LPC_RS_NNTP_LOG_DIR")
        logs = Path(logs).resolve() if logs else None
        if logs:
            logs.mkdir(parents=True, exist_ok=True)
        results = []
        with tempfile.TemporaryDirectory(prefix="lima-nntp-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            fixture_dir = library / TEST_PATH.lstrip("/")
            shutil.copytree(FIXTURES, fixture_dir)
            source = (library / "contrib/nntp.c").read_text(encoding="utf-8")
            self.assertEqual(source.count("clone_object(SOCKET,"), 1)
            (fixture_dir / "client.c").write_text(source.replace(
                "clone_object(SOCKET,", f'clone_object("{TEST_PATH}/transport",'
            ), encoding="utf-8")
            write_data(fixture_dir / "data")
            self.assertEqual(write_dates(fixture_dir / "dates.c"), 53)
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            for setting in (
                "LPC_LIB_DIR=lib\n", "LPC_NULL_MODE=distinct\n",
                "LPC_MASTER_OBJECT=/secure/master.c\n", "LPC_MAX_EXECUTION_TIME=4000\n",
                "LPC_CALL_OUT_PRESERVE_PLAYER=true\n",
            ):
                self.assertIn(setting, settings)
            settings = settings.replace("LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n")

            def execute(target, expected=None, zone="UTC"):
                check = expected is None
                profile = temporary / "lpc-rs.env"
                profile.write_text(settings if check else settings.replace(
                    "LPC_MASTER_OBJECT=/secure/master.c\n", f"LPC_MASTER_OBJECT=/{target}\n"
                ), encoding="utf-8")
                command = [str(COMPILER), "-c", str(profile)]
                command += ["--check"] if check else []
                command += [str(library / target)]
                started = time.monotonic()
                result = subprocess.run(
                    command, cwd=temporary,
                    env={"PATH": os.defpath, "TOKIO_WORKER_THREADS": "2",
                         "RUST_MIN_STACK": "16777216", "TZ": zone},
                    capture_output=True, text=True, timeout=40, preexec_fn=process_limits,
                )
                output = result.stdout + result.stderr
                labels = [line for line in output.splitlines() if line.startswith("NNTP_OK: ")]
                row = {"target": target, "compile_only": check, "zone": zone,
                       "command": command, "exit_code": result.returncode,
                       "seconds": round(time.monotonic() - started, 3), "assertions": len(labels)}
                results.append(row)
                if logs:
                    name = target.replace("/", "-") + "-" + ("compile" if check else zone.replace("/", "-"))
                    (logs / f"{name}.log").write_text(output, encoding="utf-8")
                    (logs / "results.json").write_text(json.dumps({
                        "source_sha256": hashlib.sha256(source.encode()).hexdigest(),
                        "results": results,
                    }, indent=2) + "\n", encoding="utf-8")
                self.assertEqual(result.returncode, 0, output)
                if check:
                    self.assertEqual(output, "")
                else:
                    self.assertEqual(len(labels), expected, output)
                    self.assertEqual(len(set(labels)), expected, output)
                    self.assertEqual(output.splitlines()[-1], f"NNTP_TOTAL: {expected}", output)
                    self.assertEqual(len(output.splitlines()), expected + 1, output)

            for target in (
                "contrib/nntp.c", "port_tests/nntp/client.c", "port_tests/nntp/transport.c",
                "obj/secure/socket.c", "daemons/news_d.c", "obj/mudlib/newsreader.c",
            ):
                with self.subTest(compile=target):
                    execute(target)
            for name, expected in (("framing", 235), ("commands", 15), ("posting", 29), ("terminal", 25)):
                with self.subTest(runtime=name):
                    execute(f"port_tests/nntp/{name}.c", expected)
            for zone in ("UTC", "America/Los_Angeles", "Asia/Kathmandu"):
                with self.subTest(zone=zone):
                    execute("port_tests/nntp/dates.c", 53, zone)
            self.assertEqual(sum(row["assertions"] for row in results), 463)


if __name__ == "__main__":
    unittest.main(verbosity=2)
