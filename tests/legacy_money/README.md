# Optional legacy money regression tests

Build the sibling lpc-rs workspace, then run from this checkout:

```sh
python3 tests/legacy_money/test_legacy_money.py
```

Set `LPC_RS_COMPILER` and `LPC_RS_DRIVER` to select other built binaries. The
standard-library runner compiles eight programs and checks **39 LPC assertions**
in a temporary copy of the actual library, using POSIX resource limits. It keeps
distinct nil, preserved call-out players and the 4000ms limit. The driver uses
loopback port zero without clients or external connections, and the test copy
is removed afterward.

The test constructs actual MONEY piles and uses real get verb methods, MONEY_D
and body/pockets. An OBJ actor supplies capacity/user/message boundaries; the
master permits local fixture currency creation/saves. Cases check zero/two-argument
pickup, description callbacks, aliases, merges into existing/missing denomination
entries, partial/all/whole pickup, ambiguous generic requests, invalid pickup
quantities, decimal wallet credit and recovery of seeded values across currencies.

MONEY stores integer counts per denomination. Named `all` takes only that
denomination; numeric `coin`/`coins` counts the sole stored denomination, and
multiple denominations require a choice even if they share one currency.
Whole-object get recovers every denomination. This differs from the selected
COINS balance model, which remains unchanged by this adapter.

USE_MONEY stays unselected: the master rejects that define, and the default
drop/loot path continues to use COINS. The suite does not exercise alternate
USE_MONEY drop, mixed-provider drop/merge interoperability, parser routing,
real login, concurrency or account restart. It qualifies direct construction,
merge and pickup of the optional source without activating another profile.
Existing wallet/payment/report work is not repeated.
