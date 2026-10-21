# Body-stat compatibility checks

Run from the Lima checkout:

```sh
python3 tests/body_stats/test_body_stats.py
```

Build the adjacent lpc-rs release compiler first, or set `LPC_RS_COMPILER` to an
existing binary. The runner copies the library into a temporary directory and
changes only the BODY_STATS selector between the shipped LIMA, DND and RIFTS
choices. Every choice uses distinct nil, the persistent profile's execution
limit and the selected combat/simple-advancement modules. Production remains
BODY_STATS_LIMA.

Eighteen compile checks include the actual adversary, body, menu and user under
each selector, plus two fixture programs. The actual menu discovers and loads all
race sources, then advertises five Lima, two DND or two Rifts races. Actual race
and body constructors run through a small fixture at USER_OB, preserving the
body's caller-path check without an account or connection. The master fixture
allows local file and object operations. The real stat providers, class schema,
hooks, body setup and supporting daemons remain in use.

The three executions pass 124/56/60 assertions (240 total): race filtering and
identity, absent stat/modifier defaults, pre-roll spare points, missing modifier
increments, sixteen initialization rounds per race, roll bounds, zeroed modifiers,
class-returning race overrides, display, query-copy isolation, negative modifiers,
bonus hooks, resets and clone cleanup. Roll checks assert the existing formulas'
possible ranges; they do not estimate distributions or add racial adjustments.

This runs the CLI, without a driver session, sockets, account setup, login or
native-save/restart qualification. Deferred call-outs do not execute in these
single-task checks. Introductions, alternative combat/advancement choices and
complete DND/Rifts rulesets retain separate scope. Temporary libraries are removed
on completion; no scripts or build artifacts are written under local.
