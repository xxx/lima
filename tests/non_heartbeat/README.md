# Non-heartbeat pulse regression checks

Run from the dedicated Lima port checkout after building lpc-rs:

```sh
python3 tests/non_heartbeat/test_non_heartbeat.py
```

Set LPC_RS_COMPILER to use another compiler executable. The runner copies the
library into /tmp, checks four selected-heartbeat hosts, then selects the optional
non-heartbeat module there and checks eight real consumers and two fixtures.
Distinct nil, preserved player, 4000ms, typed blows and limb providers remain.
Persistent configuration is unchanged. Each CLI process has two workers, a 4GiB
address limit, 40s timeout and 16MiB stack; no build or listener is started.

Fourteen compiler checks and 37 runtime assertions cover single timer ownership,
continuation/start/stop/restart, manual/automatic transitions, jitter bounds,
penalty, callback invocation, actor context and restoration on success/error.
Fixture actors inherit real OBJ and the optional pulse. They record attacks,
then use inherited removal hooks and native destruction to check timer cleanup.
An unrelated timer verifies that stopping combat cancels only the combat timer.
Fixtures observe the native player with efun::this_player because Lima's simul
deliberately rejects the obsolete this_player interface.

The CLI checks cancel a pending row before explicitly invoking its callback,
matching one-shot removal order. All fixture timers are cancelled or destroyed
before the initializer commits. They do not exercise elapsed scheduling, fresh
callback transactions, uncaught callback failure/retry, complete adversary/body
combat, death or saves. An error caught by the fixture retains transaction
writes; its queued successor is not proof of uncaught-error rescheduling.

Manual speed zero preserves the existing target/swing policy. Entering it
cancels the pending timer, while the existing attacking flag is retained;
switching back to a nonzero interval rearms if still attacking. A nonzero change
retains the current deadline and affects the next interval. Default interval,
jitter, negative-delay handling and penalty remain.
