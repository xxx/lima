# Rifts advancement compatibility checks

Run from the dedicated Lima port checkout after building lpc-rs:

```sh
python3 tests/rifts_advancement/test_rifts_advancement.py
```

Set LPC_RS_COMPILER to use another compiler executable. The runner copies the
library into /tmp and first checks four hosts under selected simple advancement.
It then selects ADVANCEMENT_RIFTS only in the copy, checking actual hosts and
fixtures with Lima and Rifts stats. There are 25 compiler checks and two CLI runs
with 17 runtime assertions each. Distinct nil, preserved player, 4000ms, USE_MASS
and the selected corpse/limb/heartbeat/typed-blow/skills providers remain.
Production remains simple advancement and Lima stats. Each CLI process has two
workers, a 4GiB address limit, a 40s timeout and a 16MiB stack; no Cargo build or
listener is started.

Actual NPC and human body constructors run. A small USER_OB fixture makes the
body clone through its required caller path without a connection or account.
The real stat, health and advancement providers exercise karma conversion and
clamping, level/XP conversion, first and later HP bounds, roll reuse, actual
limb-health refresh and SDC arithmetic. Rifts supplies PE through its actual stat
setter/refresh methods; Lima has no PE stat. This records the pairing difference
without inventing a conversion or imposing a new selector guard.

The suite preserves the existing HP-cache behavior explicitly: after caching a
higher level, a lower-level query includes one extra roll through the inclusive
slice. Actual set_level applies that result to health. An explicit cache reset
and body-style refresh return to one PE-based roll. This is a retained mudlib
issue for a separate decision; these tests do not endorse its gameplay policy.
The karma assertion distinguishes conversion of the complete sum from conversion
of the increment alone.

Delayed setup_monster is cancelled. Fixtures run synchronously in one initializer
transaction, cancel timers and use efun::destruct for cleanup. This does not test
complete XP-command or linked-save paths, ambient set_experience, account/login/
logout, persistence, combat/rewards, elapsed scheduling, retries or concurrent
players. LIMB_SHIELDS stays disabled; shield adjustment is arithmetic coverage.
