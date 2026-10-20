# Web-editor backend regression tests

After building the sibling lpc-rs workspace, run from this checkout:

```sh
python3 tests/web_editor/test_web_editor.py
```

Set `LPC_RS_COMPILER` and `LPC_RS_DRIVER` to select other built binaries. The
standard-library Python runner uses POSIX resource limits and a temporary copy
of the actual library. It retains distinct nil, preserved call-out players and
the 4000ms task limit. The temporary driver binds to loopback port zero without
client connections; no HTTP request or external endpoint is used. Temporary
files are removed afterward.

Seven complete programs compile, then **20 LPC assertions** run through the
actual CGI wrapper, backend, account/privilege helpers and `crypt`. The fixture
registers a synthetic offline wizard through the existing helper and writes its
account map with native save APIs. No production helper or backend is replaced.
The fixture master permits the file operations; it does not qualify the real
master's path policy.

Checks cover initial presentation, ordinary/empty/new/directory loads, save
target state, reload and retained inherited-file loading, save/CRLF/overwrite/
empty output, failed-save message and draft preservation, continued use after
failure, normal missing/incorrect fixture-login refusal and UTF-8/entity text.
Empty and new files retain the existing editor behavior: empty strings are
truthy. Python independently verifies four committed byte sequences and three
absent unsaved/refused targets after shutdown.

These are local form/file checks, not browser, HTTP transport, live login,
admin-specific flows or arbitrary malformed-form coverage. Existing markup,
credential policy and optional-service activation remain unchanged. The test
does not enable the HTTP daemon in the configured preload.
