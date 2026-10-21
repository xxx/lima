# Optional NNTP client tests

Build the sibling lpc-rs workspace, then run from this checkout:

```sh
python3 tests/nntp/test_nntp.py
```

Set `LPC_RS_COMPILER` to select another built compiler. The standard-library
runner compiles six programs and executes seven CLI fixtures with **463 LPC
assertions** in a disposable copy of the actual library. It keeps distinct nil,
preserved call-out players and the 4000ms budget, with POSIX memory/process limits.
Set `LPC_RS_NNTP_LOG_DIR` to retain command/result JSON and individual output logs;
otherwise the suite leaves no artifacts behind.

The actual contrib/nntp source is compiled unchanged. For runtime checks, the
runner substitutes exactly its SOCKET constructor path with a recording
transport. There is no production test API or separate maintained client copy.
The temporary fixture is also the master because the CLI does not bootstrap one;
configured simul efuns and the audit include remain. The CLI exits after create
runs. No gamedriver, socket, listener, DNS lookup or NNTP endpoint is used.

The recording transport records address/style and UTF-8 writes, feeds read/close
callbacks, advances the write producer on explicit drain and retains its object
after removal. Its limited owner/frame checks do not qualify the real provider's
privilege, backpressure, concurrency or native-event behavior.

Cases cover every split of a fixed greeting/article transcript, single characters,
CRLF/terminator boundaries, coalesced replies, empty bodies/LIST, Unicode and dot
framing; every public command family and callback shape; numeric GROUP/STAT
fields; queued POST/IHAVE continuations and final replies; more than 64 output
frames, file endings and rejections; QUIT, EOF, callback removal/throw, stale
callbacks, send failures and native string limits. Final replies may precede the
last drain notification after the complete body is queued. Python datetime in
UTC supplies 53 fixed date vectors repeated in three host zones, including
negative epochs, leap days/centuries and DST boundaries.

LIST returns word arrays; article-family/HELP/date responses retain raw arrays
including the status line. POST/IHAVE retain raw continuation and final status
callbacks. XHDR remains explicitly unimplemented and reports its result in queue
order without issuing a command. Terminal failure clears pending requests before
one connection-level error; explicit removal is silent. The date formatter keeps
the two-digit YYMMDD HHMMSS interface and its century ambiguity.

These checks qualify the optional client source, not live interoperability or
full RFC conformance. NNTP_OB remains unchanged, local NEWS_D/newsreader remain
selected, and no alternate service is activated. Source references for framing
and upload ordering are RFC 3977 sections 3.1.1/6.3.1/6.3.2 and legacy RFC 977,
reviewed 20 October 2026; see the port README/native comparison for context.
