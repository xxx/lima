# Coin denomination and value-conservation tests

Build the sibling lpc-rs workspace, then run from this checkout:

```sh
python3 tests/coins/test_coins.py
```

Set `LPC_RS_COMPILER` and `LPC_RS_DRIVER` to select other built binaries. The
standard-library Python runner uses POSIX resource limits, compiles ten programs
and runs **55 LPC assertions** in a temporary copy of the actual library. It
retains distinct nil, preserved call-out players and the 4000ms limit. The driver
listens on loopback port zero without clients or external connections; the
runner removes its temporary copy afterward.

The fixture calls actual get/drop verb methods and COINS through the real money
daemon and body/pockets module. An OBJ actor supplies capacity/user/message
boundaries, and a permissive master permits fixture currency creation/saves.
Two currencies use base units, quarters and tenths. Wallet assertions compare
against fixed expected totals; seeded world piles are recovered to prove that
transfers conserve the combined value. Parser routing, real login, concurrency,
account restart and shop/payment/report behavior are outside this fixture.

Coverage includes new piles, merging into existing same/different currencies,
rendering denominations, plural aliases, partial/whole/all pickup, full fractional
wallet drops, ambiguous mixed-currency requests, repeated decimal transfers,
ordinary base-unit behavior, float-valued constructor piles, insufficient/zero/
negative requests and absent currencies. A refusing destination verifies cleanup
without a debit, and a refusing merge subclass verifies that a caught exception
occurs before the wallet debit. Those fixtures refuse before changing pile state;
arbitrary callbacks that mutate state before failing are not qualified here.

COINS constructor/merge amounts remain base-currency values. Drop converts an
integer denomination count before storing it, and get converts a requested count
against the stored currency. `all` transfers that currency's entire balance,
including fractions. Generic `coin`/`coins` with a numeric amount retains its
base-unit meaning and requires a single currency. The optional USE_MONEY branch
and legacy std/money provider remain unselected and are not run by this test.
