# CGI helper regression tests

After building the sibling lpc-rs workspace, run from this checkout:

```sh
python3 tests/cgi/test_cgi.py
```

Set `LPC_RS_COMPILER` to use another built `lpc-rs-lpcc` executable. The runner
uses Python's standard library and POSIX resource limits.

The test copies the current library into a temporary directory and uses its
configured distinct nil mode and call-out player setting. It compiles the two
actual CGI helpers, then runs 42 LPC assertions covering documentation pages,
inheritance links and finger-page output. It exercises both
the GET-style calls and the finger form's data-plus-headers call.

A controlled finger-data provider supplies records without starting network
services or creating accounts. The real class definitions, grammar, glob,
ANSI translations and simulated efuns remain in use. Documentation files and
a small inheritance tree are fixture data. This tests helper output; it does
not test HTTP transport or the finger daemon's access policy.
All fixture writes stay in the temporary copy, which is removed after the test.

Upstream 1.1a4 removed the demo-site `mudinfo.c` helper. Its 13 assertions and
IMUD fixture were retired with that source; the remaining coverage is unchanged.
