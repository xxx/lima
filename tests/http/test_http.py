"""Exercise selected HTTP request framing through an in-memory socket recorder."""

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
TEST_PATH = "/port_tests/http"


def process_limits():
    resource.setrlimit(resource.RLIMIT_AS, (4 * 1024**3, 4 * 1024**3))


def write_data(library, directory):
    directory.mkdir()
    (library / "WWW/framing.html").write_bytes(b"HTTP_FRAGMENT_OK\n")
    (library / "WWW/framing.bin").write_bytes(bytes(range(256)) * 16)
    (directory / "cgi-get.expected").write_bytes("CGI é\r\n\r\n".encode("utf-8"))
    header = "POST /scgi/port_http.c HTTP/1.0\r\n" + "".join(
        f"X-Block-{index}: " + "x" * 200 + "\r\n" for index in range(100)
    ) + "Content-Length: 10\r\n\r\nname=value"
    body = "&".join(f"field{index}=v{index:04}" for index in range(1000))
    post = ("POST /scgi/port_http.c HTTP/1.0\r\nX-Long: streamed\r\n"
            f"Content-Length: {len(body)}\r\n\r\n" + body)
    (directory / "large-header.request").write_bytes(header.encode("ascii"))
    (directory / "large-body.request").write_bytes(post.encode("ascii"))


class HttpTests(unittest.TestCase):
    def test_request_framing(self):
        self.assertTrue(COMPILER.is_file(), f"Build lpc-rs first: {COMPILER}")
        logs = os.environ.get("LPC_RS_HTTP_LOG_DIR")
        logs = Path(logs).resolve() if logs else None
        if logs:
            logs.mkdir(parents=True, exist_ok=True)
        results = []
        with tempfile.TemporaryDirectory(prefix="lima-http-test-") as directory:
            temporary = Path(directory)
            library = temporary / "lib"
            shutil.copytree(ROOT / "lib", library)
            fixture_dir = library / TEST_PATH.lstrip("/")
            shutil.copytree(FIXTURES, fixture_dir)
            source = (library / "secure/daemons/http_d.c").read_text(encoding="utf-8")
            constructor = "clone_object (SOCKET,"
            self.assertEqual(source.count(constructor), 1)
            (fixture_dir / "daemon.c").write_text(source.replace(
                constructor, f'clone_object ("{TEST_PATH}/socket",'
            ), encoding="utf-8")
            shutil.copy2(FIXTURES / "gateway.c", library / "secure/cgi/port_http.c")
            write_data(library, fixture_dir / "data")
            settings = (ROOT / "lpc-rs.env").read_text(encoding="utf-8")
            for setting in (
                "LPC_LIB_DIR=lib\n", "LPC_NULL_MODE=distinct\n",
                "LPC_MASTER_OBJECT=/secure/master.c\n", "LPC_MAX_EXECUTION_TIME=4000\n",
                "LPC_CALL_OUT_PRESERVE_PLAYER=true\n",
            ):
                self.assertIn(setting, settings)
            settings = settings.replace("LPC_LIB_DIR=lib\n", f"LPC_LIB_DIR={library}\n")

            def execute(target, check):
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
                         "RUST_MIN_STACK": "16777216", "TZ": "UTC"},
                    capture_output=True, text=True, timeout=40, preexec_fn=process_limits,
                )
                output = result.stdout + result.stderr
                labels = [line for line in output.splitlines() if line.startswith("HTTP_OK: ")]
                results.append({"target": target, "compile_only": check, "command": command,
                                "exit_code": result.returncode,
                                "seconds": round(time.monotonic() - started, 3),
                                "assertions": len(labels)})
                if logs:
                    name = target.replace("/", "-") + ("-compile" if check else "-runtime")
                    (logs / f"{name}.log").write_text(output, encoding="utf-8")
                    (logs / "results.json").write_text(json.dumps({
                        "source_sha256": hashlib.sha256(source.encode()).hexdigest(),
                        "results": results,
                    }, indent=2) + "\n", encoding="utf-8")
                self.assertEqual(result.returncode, 0, output)
                if check:
                    self.assertEqual(output, "")
                else:
                    self.assertEqual(len(labels), {"get": 465, "post": 1808, "stream": 309}[Path(target).stem], output)
                    self.assertEqual(len(labels), len(set(labels)), output)
                    self.assertEqual(output.splitlines()[-1], f"HTTP_TOTAL: {len(labels)}", output)
                    self.assertEqual(len(output.splitlines()), len(labels) + 1, output)

            for target in (
                "secure/daemons/http_d.c", "port_tests/http/daemon.c", "port_tests/http/socket.c",
                "obj/secure/socket.c", "WWW/cgi/formtest.c", "secure/cgi/webed.c", "secure/master.c",
            ):
                with self.subTest(compile=target):
                    execute(target, True)
            for name in ("get", "post", "stream"):
                with self.subTest(runtime=name):
                    execute(f"port_tests/http/{name}.c", False)
            self.assertEqual(sum(row["assertions"] for row in results), 2582)


if __name__ == "__main__":
    unittest.main(verbosity=2)
