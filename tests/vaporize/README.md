# NPC vaporization regression checks

Run from the dedicated Lima port checkout after building lpc-rs:

```sh
python3 tests/vaporize/test_vaporize.py
```

Set LPC_RS_COMPILER to use another compiler executable. The runner copies the
library into /tmp, checks four corpse-profile hosts and then selects
DEATH_VAPORIZE only in that copy. Eight real consumers and five fixtures compile
there, for 17 compiler checks. Distinct nil, preserved player, 4000ms, USE_MASS,
heartbeat, skills, typed damage and limb providers remain. Persistent selectors
are unchanged. Each CLI process uses two workers, a 4GiB address limit, a 40s
timeout and a 16MiB stack; no Cargo build or listener is started.

One CLI execution verifies 22 assertions with actual ADVERSARY, CONTAINER,
ARMOUR and OBJ constructors. It covers mixed worn/unworn armour and an ordinary
item, blocked worn transfer, one-second callback registration and invocation,
NPC removal, item preservation, worn-only unwear calls, timer cleanup, no corpse,
and the empty-inventory case. The armour fixture counts do_remove calls while
delegating to the real implementation. The ordinary item sets its protected mass
inside setup, as stock objects do.

The fixture cancels delayed setup_monster to isolate the death provider. It
calls the protected death hook through a public wrapper, consumes its pending
one-shot row and explicitly invokes the captured callback in one transaction.
All fixture objects and timers are removed before commit.

These checks do not run elapsed scheduling or health kill_us/reward/party/karma,
ghost marking, active effects, player ghost routing, connected-user messaging,
arbitrary transfer refusals or saves. They preserve the existing delay and
combat-stop policy. Vaporize's player drop_corpse hook remains integration-required;
production stays on corpse death. No general destruction policy changes.
