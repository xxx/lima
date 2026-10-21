# HTTP request framing tests

Build the sibling lpc-rs workspace, then run from this checkout:

```sh
python3 tests/http/test_http.py
```

Set `LPC_RS_COMPILER` to select another built compiler. The standard-library
runner performs **seven compiler checks and 2,582 LPC assertions in three CLI
executions**, with distinct nil, preserved call-out players and the 4000ms budget.
POSIX resource limits cap memory and process time. `LPC_RS_HTTP_LOG_DIR` optionally
retains command/result JSON and output logs; default runs leave no artifacts.

The suite copies the actual library into a disposable directory, compiles the
selected HTTP daemon unchanged, and substitutes only its SOCKET constructor
path for runtime checks. The fixture is also a temporary permissive master;
configured simuls and audit include remain. The recording socket feeds byte
callbacks, records output and invokes the existing writer on explicit drain.
No real socket/listener, gamedriver, endpoint or service activation is used.

Coverage includes every split position in a normal GET and in POST requests with
both close/keep-alive headers; split CRLF, byte-by-byte reads, LF-only compatibility,
coalesced headers/body, exact Content-Length boundaries, empty bodies, interleaved
peers and EOF before completion. It checks exact static binary output, UTF-8 CGI
response byte lengths and CRLF conversion, and the existing body-only POST reply.
The CGI recorder observes exact form/header arguments without running web editor
operations. Large aggregate headers and a thousand-field body exercise fragment
assembly beyond the concatenation limit, keeping individual fields small. Large
packets and byte oracles are generated under the temporary directory.

The seven compile targets retain the real daemon, SOCKET, master, form example
and web-editor CGI entry, plus the recording daemon/socket. No separate HTTP
implementation or production test API is maintained. The previous daemon fails
all three runtime fixtures while retaining its compiler successes.

The daemon still serves one request per connection and closes; a keep-alive
header does not enable persistent connections or pipelining. Existing per-value
limits, form decoding, CGI signatures and response conventions remain. These
checks qualify callback-level request framing, not live interoperability, full
HTTP conformance, real provider privilege/ownership, native event ordering or
backpressure. HTTP stays disabled in the configured preload.
