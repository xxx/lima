# Contributed transient-effect tests

After building the sibling lpc-rs workspace, run from this checkout:

```sh
python3 tests/transient_effects/test_transient_effects.py
```

Set `LPC_RS_COMPILER` and `LPC_RS_DRIVER` to select other built binaries. The
standard-library Python runner uses POSIX resource limits and a temporary copy
of the actual library. It retains distinct nil, preserved call-out players and
the 4000ms task limit, and binds a temporary driver listener to loopback port zero.
It compiles the real contributed programs and selected transient references,
then runs 36 LPC assertions through a permissive fixture master. No production
effect implementation is replaced. The temporary library is removed afterward.

Checks cover two-second native repeating callbacks, apply-before-expiry duration,
nil/detached/nonliving hosts, pause/restart, self movement, independent identities,
merge composition, strength clamping, self-removal, cancellation, configured player
capture and the existing duration-only save registration. Real elapsed dispatch
gives a duration-two effect three applications before removal. The player-context
check uses a fixture object as its player identity, rather than a logged-in user.

The drunkenness receiver explicitly implements the older `stun_us` contract,
records healing and checks all amounts against strength bounds. It accepts only
the example's one-argument command names and drool action; random branches are
validated when selected, without requiring a particular random sequence. These
are callback checks, not actual combat healing, stun or command execution.
The selected `/std/body` still has no `stun_us`; the fixture confirms that limit
instead of silently substituting its different `stun(limb, period)` policy.
Full player behavior, restoration and modern `/std/transient` runtime behavior
are outside this focused test.
