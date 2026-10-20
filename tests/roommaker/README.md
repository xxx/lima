# Room-maker regression tests

After building the sibling lpc-rs workspace, run from this checkout:

```sh
python3 tests/roommaker/test_roommaker.py
```

Set `LPC_RS_COMPILER` and `LPC_RS_DRIVER` to select other built binaries. The
standard-library Python runner uses POSIX resource limits and a temporary copy
of the actual library. It retains distinct nil, preserved call-out players and
the 4000ms task limit. Both driver processes bind to loopback port zero without
client connections. Temporary files are removed afterward.

The test passes **63 LPC assertions and 15 compiler checks**. The first process
runs 46 assertions through actual `CMD.call_main`, the contributed menu, old-menu
callbacks, text editors and file APIs. Fixture objects provide user/body/shell
identities, screen width, flags, output collection and a recording modal stack.
No production command, menu or editor implementation is replaced.

Checks cover room/submenu navigation; light, weather, include, description and
file prompts; item/exit editors and their completion/abort paths; bound object
quantities; views/deletions; successful saves, overwrite and failed saves; two
independent menu owners; and menu/editor clone cleanup. Empty item/exit editors
must not introduce a leading blank line. Failed writes must leave earlier output
intact and permit continued menu use.

The runner compiles six generated rooms and starts a fresh driver for 17 further
assertions. These construct the actual room providers, item and object children.
They check defaults, indoor/outdoor/reset selection, light and weather flags,
exact quotes/backslashes/newlines, item identifiers, exits, object quantities and
independent room contents. Python also checks include de-duplication, overwrite
and the exact output-file set. Native save encoding is used only for the emitted
integer/string subset; the test does not imply that every saved value is valid
LPC source.

These fixtures do not establish logged-in user/inputsys behavior, full shell
parsing, permission policy, pager/completion interactions, live weather output,
logout or arbitrary input validation. They test the contributed consumer; the
previously qualified shared menu/editor frameworks remain unchanged.
