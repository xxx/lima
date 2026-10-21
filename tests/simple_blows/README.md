# Core simple-damage regression checks

Run from the dedicated Lima port checkout after building the native workspace:

```sh
python3 tests/simple_blows/test_simple_blows.py
```

Set LPC_RS_COMPILER to use another compiler executable. The test copies the
library into /tmp, selects BLOW_TYPES and then BLOW_SIMPLE there, and uses the
checkout's distinct-nil, preserved-player profile. Persistent selectors stay
unchanged. Each compiler process has two workers, a 4GiB address limit, a 40s
timeout and a 16MiB stack. No Cargo build or listener is started.

The suite performs 22 compiler checks of actual adversary/body/human/trainer,
armour/weapon/dummy/shield sources and three component fixtures. Two CLI runtime
executions verify 33 typed-path and 32 simple-path assertions. They cover actual
queue producers, class limb checks, diagnostic formatting, natural armour,
armour reduction/clamping, action preservation, damage dispatch and real
M_DURABILITY wear on both armour and weapon components.

Simple damage remains scalar and uses the sink's existing untyped fallback
calculation. Typed damage retains its existing resistance, weakness and fallback
rules, including signed-reduction durability behavior. Deterministic armour
settings test exact amounts; a separate loop checks natural-armour random bounds.

The health receiver records damage instead of applying a full character model.
Unused gameplay/persistence hooks throw; the durability components are not
complete inventory objects. These checks do not qualify complete combat, death,
wearing/unwearing, saved characters, type-specific special weapons or spell
effects. Shield compilation verifies the formatter interface only; its typed
effect callback is not exercised or adapted.
