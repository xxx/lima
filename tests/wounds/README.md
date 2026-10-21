# Wound-health regression

Run from the Lima checkout with current adjacent release binaries:

```sh
python3 tests/wounds/test_wounds.py
```

`LPC_RS_COMPILER` and `LPC_RS_DRIVER` can select other existing binaries. The
runner copies the library into a temporary directory and retains distinct nil,
preserved call-out players, 4000ms, USE_MASS and the selected statistics/combat
modules. Only HEALTH_STYLE changes to HEALTH_WOUNDS in that copy; production
remains HEALTH_LIMBS.

Four compiler checks retain the selected limb-health host chains. Ten more
check actual wound-health adversary/body/human/trainer, diagnose verb, menu and
user sources, then the test actor/master and USER_OB constructor caller.

The real driver executes manual damage/healing and a real fifteen-second wound
callback. Assertions cover diagnosis, multiple wound types, partial/full healing,
small-wound coalescing and damage totals, one pending timer, maximum-health resets,
limb disable, manual periodic/reincarnation updates, the death cancellation
boundary, destruction, firing/rearming and cancellation through elapsed dispatch.
They also retain the inherited generic diagnosis helper and configured player
context. Each actor uses actual human/body/limb/wound code; an inherited wrapper
counts periodic calls without replacing the wound algorithm.

A fixture master permits local object/file operations. A small USER_OB fixture
calls the actual body constructor at its required caller path. The actor's death
hook throws a recognized test exception after the actual death state/cancellation
boundary, isolating corpse/reward/account behavior. The runner does not qualify
login, saved-data migration, full combat/death rules or other configurations.
Normal elapsed healing is held at zero for deterministic accounting checks;
explicit healing and the original random wound-update formula still run.

The driver requires an ephemeral listener on 127.0.0.1 but the fixture makes no
connections or external requests. The temporary library is removed on completion.
No diagnostic scripts or build output are written under local.
