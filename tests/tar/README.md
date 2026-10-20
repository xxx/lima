# Tar writer regression test

Build the sibling lpc-rs workspace, then run from this checkout:

```sh
python3 tests/tar/test_tar.py
```

The test needs Python's standard library, GNU tar and POSIX resource limits.
`LPC_RS_COMPILER` and `LPC_RS_DRIVER` can select other built executables.
It copies the current library under `/tmp`, keeps the distinct-nil,
preserved-player, 4000ms profile, and uses real simuls/includes with a temporary
permissive master. The driver binds an ephemeral loopback listener; no client or
external service is used. All runtime files and the sparse size-limit fixture
are removed afterward.

The actual contributed helper produces six concurrent archives through elapsed
one-shot call-outs. Twenty-one LPC assertions cover deferred creation,
truncation, relative paths, input/output validation, exact UTF-8 byte limits, independent jobs,
partial large-file progress, retirement and an empty-directory-only work list.
Python tarfile checks every member's name, kind, metadata, exact payload, padding
and footer; GNU tar checks listings and reads representative payloads to stdout.
Inputs include hidden files, nested/empty directories, empty files, 1/511/512/513
byte payloads, 100-byte ASCII/UTF-8 names, and a 16 MiB + 513 byte payload that
crosses the old record-mask boundary. The output inside the archived tree is
excluded. Invalid root inputs preserve the existing output and source.

The writer queues at most ten entries or 1 MiB of payload per callback, with a
one-second continuation delay. The native `read_bytes` implementation currently
materializes the entire source before slicing; this test proves continuation
and archive correctness, not bounded physical read memory or I/O. Directory
listings are also collected in full. The test uses a permissive master rather
than qualifying a player command or live service.
