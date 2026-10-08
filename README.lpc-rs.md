# Lima for lpc-rs

This is the working lpc-rs port of Lima, based on upstream revision
`eefaff66adcae9b01e16059574c925b0d63e70a1`. The mudlib sources are in `lib/`.

Use `lpc-rs.env` from this checkout's root. It selects distinct nil, preserved
call-out player context, and a 4,000 ms task limit. Its library path is relative
to this directory.

With the lpc-rs checkout beside this one and its CLI built, check a command with:

```sh
../lpc-rs/target/debug/lpc-rs-lpcc -c lpc-rs.env --check lib/cmds/player/mudlist.c
```

The current focused checks compile 64 of 67 player commands and all 12 supporting
targets. Mudlist passes 13 runtime cases using both actual shells. `su` is next;
experimental `random2` remains deferred and `suicide` is disabled upstream.
Full-inventory and startup results are recorded separately in the port notes.

Current compatibility and handoff notes are in the neighboring lpc-rs checkout:
`doc/driver-comparison.md` and `local/lima/compatibility.md`. Temporary test
harnesses and execution logs belong in `/tmp/`. Continue source changes in this
checkout rather than making another source copy under `local/`.
