# Queue and stack regression tests

After building the sibling lpc-rs workspace, run from this checkout:

```sh
python3 tests/collections/test_collections.py
```

Set `LPC_RS_COMPILER` to select another built `lpc-rs-lpcc`. The runner uses
Python's standard library and POSIX resource limits. It copies the current
library into a temporary directory, keeps the configured distinct nil mode,
and compiles the actual contributed helpers before executing 37 LPC assertions.
The temporary library is removed afterward.

The queue tests cover fixed-capacity windows with nil padding, newest-first
member lookup, oldest-first listing, repeated wraparound, capacity one, fresh
result arrays, shared nested values and independent instances. The zero-capacity
queue retains its empty listing behavior. The stack tests cover LIFO ordering,
existing overflow/underflow errors, depth preservation, independent instances,
zero-capacity overflow and mixed values including nil, numeric zero, arrays,
mappings, floats, objects and functions. The harness inherits both actual
helper programs; it supplies no replacement collection implementation.
