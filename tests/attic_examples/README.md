# Attic example regression tests

After building the sibling lpc-rs workspace, run from this checkout:

```sh
python3 tests/attic_examples/test_attic_examples.py
```

Set `LPC_RS_COMPILER` and `LPC_RS_DRIVER` to select other built binaries. The
standard-library Python runner uses POSIX resource limits and a temporary copy
of the actual library. It retains distinct nil, preserved call-out players and
the 4000ms task limit. The checked configuration uses `USE_MASS`; alternate build
options are not covered. A temporary driver listener binds to loopback port zero
without client connections, and the temporary library is removed afterward.

Eight compiler checks cover both examples, their container/outdoor providers,
the shipped table references and fixtures. The driver runs **60 LPC assertions**
against the actual adapted sources with real simuls/includes and a permissive
fixture master. The weighted fixture item adds only typed mass and an identifier;
the examples and their movement/container/exit implementations are not replaced.

Eleven table checks cover construction, 64.0 mass, attachment, identity, the
default `on` relation, empty stock and 64-unit surface capacity. Real item moves
exercise default placement, capacity accounting, overflow refusal preserving
the source environment, capacity release and refusal of the unsupported `in`
relation. Forty-eight town checks cover every subset of north/east/south/west,
with exact titles, description ordering and included/omitted exit paths. A final
assertion checks clone cleanup.

The zero-direction case retains its existing `Intersection` title and
`The road leads .` description. Exit paths retain the sibling `attic/town/`
prefix, but that destination directory is absent and `attic/town.c` is TODO.
These are constructor/helper and metadata checks; real player/parser get/put/go,
town traversal and new destination content are outside this test.
