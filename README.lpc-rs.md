# Lima for lpc-rs

This is the working lpc-rs port of Lima, based on upstream revision
`eefaff66adcae9b01e16059574c925b0d63e70a1`. The mudlib sources are in `lib/`.

Use `lpc-rs.env` from this checkout's root. It selects distinct nil, preserved
call-out player context, and a 4,000 ms task limit. Its library path is relative
to this directory.

With the lpc-rs checkout beside this one and its CLI built, check a command with:

```sh
../lpc-rs/target/debug/lpc-rs-lpcc -c lpc-rs.env --check lib/cmds/player/mudlist.c
```

The current standalone-source inventory, on **16 October 2026** against native
**2870a0b4** and this checkout at **6ca00180**, compiles **957 of 1,182 .c files**:
**225 fail, with no timeouts or crashes**. All 924 previous successes remain;
**31 previously failed targets now compile** (23 junk/spawner, three vehicle and
five room/grid), plus both new virtual-room source files. This fresh sweep replaces
the dated 924/1,180 inventory. Counts include optional and inherited-only sources
and are not a completion percentage. **60 failed fragments** have reviewed passing
complete hosts; the other 165 failures still include further fragments/optional
code and are not a native-gap count. All 34 standard-room sources and 22 junk
examples compile. No target constructor or driver gameplay rerun is included.

Command compilation is player **65/67**, wizard **81/81**, guild **4/4**, verbs
**90/90**, transitional **49/49** and transitional administrator **9/9**. The two
player entries remain experimental random2 (deferred) and suicide (disabled upstream).
All 17 tracked core objects, all 22 std/classes files and all eight Pinto examples
compile. The prior bounded treefor set remains **178/178**, and admtool's set **34/34**.
All 25 admtool leaves retain navigation and selected report/edit coverage. The race
report matches six definitions; memory reports rank 274/257 objects across restart
using nonadditive estimates and separate strings within the driver's size limit.

The school batch converts all 88 sources to native strings/includes, corrects
three stale map/return-exit paths and updates 13 native example texts. All 104
final text values are checked, with 91 unchanged from upstream; 11 complete
displayed examples compile. Bounded source checks pass **109/109**. The room-chat
chance has an explicit default to preserve supplied intervals under the current
compiler; a non-trailing-default reproducer is recorded for a separate fix.

The new normal-game run visits 45 school rooms and views 31 selected lessons,
covering maps, navigation, item aliases/actions, native examples and all three
room-chat forms. It retains all 596 prior first-session command records and adds
**203 school inputs**. All 30 lifecycle checkpoints, both clean 151-preload boots,
saved login and normal exits pass. Five lessons' NPC dependencies were unvisited
in that run; their accountant, wander, Harry and vendor support is described
below.
Other historical prose/partial examples remain outside these checks. The whole-
source inventory above remains the earlier measurement.

The NPC support batch adapts 11 source files: accountant/wander/Harry/vendor,
explicit parent setup, banker lesson, required account-daemon expressions and
list/buy rule order. Native API, master policy and configuration are unchanged.
**124/124** bounded targets compile. The final normal-game run retains all **799**
preceding first-session command lines and adds **82 NPC inputs** (**881 total**).
All 30 lifecycle checkpoints, both clean 151-preload boots, saved login and normal
exits pass. All five remaining NPC lessons are visited with their actual spawns.

Banker menus, rates, deposit/withdrawal/two exchange paths, wallet/transaction
results and fresh-process account restoration pass. Dragon moves by a real
scheduled callback using public 2-second/unrestricted-area controls, then its
example settings are restored. Harry's laugh and native block-nod responses pass.
Vendor list/filter/explicit selection, sword and coloured-stick buy/sell paths
work through normal commands. The player returns empty-handed to the Grand Hall
with original wallet/pager state restored. Same six retained fixtures and two
address-only overrides; no new source fixture/helper or permission grant.

Absent/zero bank-account lookup, non-base-denomination exchange, Harry following,
duplicate/unique stock and wander-area removal retain separate source scopes.
Yakitori cashier still blocks on a private inherited call. The NPC implementation
is committed in `39da199b`. The refreshed inventory repeats compilation only;
the preceding 881-input runtime evidence retains its scope. Existing equipment
and TCP work stays closed.

The junk/material-spawner batch applies the reviewed 22-file adaptation: 26 float
weight/value arguments, void return/nil path handling and dirt configuration through
setup. Headers and 20 CRLF/two LF source formats are retained. No native API, master
policy or configuration change. **27/27 bounded targets compile**. The final run
retains the preceding gameplay checks and adds **171 inputs** (**1052 total**),
including **23 constructors**, **six delayed normal searches** and **three STATE_D
updates**. Both clean 151-preload boots, all 30 lifecycle checkpoints, saved login
and normal exits pass.

Random trash delivers configured loot, refuses repeats and regenerates after
counter expiry. Dirt delivers default/custom loot and stays exhausted after its
state update. Inventory/actor counts and cleanup pass using real handlers and
public scheduling APIs; no direct receiver edits or new LPC fixture/grant. Existing
fixtures, earlier bank restoration and other accepted checks remain. Original
message/yield/capacity/fertilizer/setup quirks retain their scopes. The full-source
924/1,180 count above remains the preceding measured inventory.

The vehicle batch adopts the reviewed four-file proposal exactly: nil string
returns, native callbacks/mixed movement payloads, object-valued driver state,
canonical enter/leave registration and explicit empty exit/dismount rules. Camaro
and shared frameworks remain unchanged; headers/LF are retained. No native API,
master-policy or configuration change. **12/12 complete targets compile**.

The final **274.44s** run retains the preceding gameplay sequence
(1056 retained command inputs) and adds **61 vehicle inputs** (**1117 total**).
Two real connected accounts exercise both constructors, outside/inside descriptions,
passenger refusal to drive, four driving round trips and driver handover in both
vehicles. All eight movements produce the expected driver views plus passenger
views or outside-observer notifications. Driver flags/object identity and final
clearing pass. The second account uses unchanged AUTO_WIZ guest status, with no
admin grant; both accounts save and log out normally. Both clean 151-preload boots,
all 30 lifecycle checkpoints, the original saved login and bank restoration pass.
Same six retained fixtures/two address overrides; no new LPC helper or grant.

The six audited room corrections are now adopted exactly: explicit format_list
separator, nil factory declines, explicit file_name receiver, BASE_ROOM constructor
forwarding, entrance server case and the tunnel's live set_default_error method.
Headers/LF remain. Original constructor/case/stale-method defects are distinguished
from native arity/type adapters in the audit. No native API, master-policy or
configuration change. **13/13 complete targets compile** from dedicated source.

The **237.05s** full replay retains the prior 1117-input gameplay sequence
(1114 replay records, 3 fewer repeated observational polls), adding **69 room inputs**
(**1183 total**). Four Labroom cases, three maze rooms, the entrance, all **64
plains cells/256 exits** and normal tunnel refusal/movement pass alongside prior
vehicle, search/state, command and banker checks. All 30 lifecycle checkpoints,
both clean 151-preload boots, saved login and normal exits pass. Same six fixtures/
two address overrides, no new helper or grant. The whole-source **924/1,180**
remains the preceding measured inventory.

The grid/maze virtual-room migration now uses the native blueprint-path contract.
Master lookup asks virtual_blueprint without constructing a room clone. The new
grid_room blueprint configures its actual requested identity through the grid
server; Labroom obtains its coordinate/direction configuration from the maze server.
A lowercase labyrinth proxy delegates to the uppercase server, sharing the same
maze state. Legacy direct object factories remain available. Six source files
change (four existing/two additions); native code, authorization hooks and
configuration stay unchanged. **14/14 complete targets compile**.

Focused **90.96s** and full **281.52s** runs pass both clean
151-preload boots, all 30 lifecycle checkpoints, saved login and normal exits.
The full replay retains the prior gameplay families (1182 records, including
69 room controls) and adds **136 virtual inputs** (**1318 total**). The known
failed-load block becomes positive loading/identity/reuse controls; other room
cases remain. All **64 plains cells/256 exits** and **49 reachable maze cells**
match server data/configuration and requested names. Thirteen normal movement
commands, lookup reuse, independent properties, two destruction/recreation cases,
no factory room clones and explicit cleanup pass. Banker restoration and both
connected-player vehicle handovers remain. Same six fixtures/two address overrides,
no new LPC test helper or grant. The whole-source 924/1,180 remains its measured
inventory; the two additions are not counted by arithmetic.

The three recorder adapters are now adopted exactly: direct per-label object
factory construction/replenishment, native callbacks/normal button press check,
and live-object container reset cleanup. Headers/LF remain. PARAM_OBJ, panel and
transcript need no edit; native code, master and configuration stay unchanged.
**14/14 complete targets compile** from dedicated source.

The final **276.80s** full replay retains the preceding gameplay families
(1321 replay records) and adds **108 recorder inputs** (**1429 total**).
Two independent recorders, normal descriptions/button/read/take paths, record/
append/overwrite transcripts, six delayed callbacks, missing-child replacement,
four surviving button identities, idempotent reset, silence and cleanup pass.
Both clean 151-preload boots, all 30 lifecycle checkpoints, saved login, banker
restoration and normal exits pass alongside the prior room/vehicle/search/state/
menu/command checks. Generated maze cases use the unchanged helper and current
layout; recorder inputs match the accepted focused audit. Same six fixtures/two
address overrides, no new LPC helper or grant. Final logs contain no failed apply
or background panic.

The earlier RST shutdown observation remains recorded separately without a native
fix/resolution claim. Generic virtual parameter names are unnecessary for these
buttons; PARAM_OBJ remains an explicit clone factory. Original playback/message
policy, attic code, equipment/TCP and reserved native discussions remain separate.
The refreshed 957/1,182 inventory above is compile-only. Guild artefact audit notes
are committed in **a77e9a4d**; the five source files now exactly match the reviewed
proposal. Native function values/partial application, retained owner-hook identity,
float values/object formatting, matching parser declaration, explicit time mode and
nil ledger argument adapt the items/menu. All headers, CRLF and final-newline states
remain. Native API/code, shared frameworks, authorization and configuration stay
unchanged.

**17/17 complete targets compile**. The final **290.14s** full replay retains the
prior meaningful 1429-input gameplay scope (1429 replay records; repeated polls
may vary), adding **71 artefact inputs**, **1500 total**. Five item constructions,
normal module installation/refusals, move-hook deduplication, blank/populated ledger,
two sorted contributors, nested game menu, mission/favour details and both
confirmation cancellations pass. Both clean 151-preload boots, all 30 lifecycle
checkpoints, saved login/bank restoration and normal exits pass with all preceding
families. Bundled guild records remain unchanged; the separate guild uses public
APIs. Same six fixtures/two address overrides, no helper or grant. Final logs have
no failed apply or worker panic.

Activation and active-buff display remain untested pending the unported mission
controller; cancellation does not qualify them. Next audit that concrete dependency.
Hidden installed modules, previous-owner hook policy and board slice/absent-leader
behavior remain original source behavior. No native feature is recommended. The
whole-source count remains its measured inventory. Equipment/TCP, native observations
and reserved discussions stay separate. Evidence: /tmp/lima-guild-artefact-20261016.


Mission-controller audit notes are committed in **75324c55**. The two source files
now exactly match the reviewed proposal: native bound delivery call-out, nil
clearing and bare arrival callback in std/modules/m_mission_control.c; bare give
dispatch in cmds/verbs/give.c. Both headers, controller CRLF/missing final newline
and give LF remain. Native code/API, master, policy and configuration stay unchanged.

**21/21 complete targets compile**. The final **325.19s** full replay retains the
prior meaningful 1500-input gameplay scope (1501 replay records; repeated polls
may vary), adding **61 mission inputs**, **1562 total**. Real room/controller reuse,
inactive refusal, actual give/two-second salvage, preserved contributor context,
manual incomplete end/delayed destruction and active-buff board rendering pass.
All 30 lifecycle checkpoints, both clean 151-preload boots, saved login/bank
restoration and normal exits pass with all earlier families. Same six fixtures/two
address overrides, no new helper or grant; final logs have no failed apply or panic.

Original favour logic still replaces the active mission and leaves the object's
expiry at time() despite a future cache. State update applies then immediately
clears XP while the cache remains; these source issues are not native gaps.
Purchase accounting, sustained/restored favours, full rewards and autonomous expiry
remain unqualified. Next review bounded favour expiry/controller-ownership
corrections. Broader game policy and native observations retain their scope.
Evidence: /tmp/lima-mission-controller-20261016.


Controller/give support is committed in **6816385f**. Two further LPC corrections
fix the observed favour lifecycle issues: guild_d supplies object/cache the same
absolute expiry and uses that deadline on restore/cleanup; std_favour preserves
the actual mission and resolves its controller lazily. Headers/LF/final-newline
state remain. No native API/code, master, privilege, configuration or fixture change.

**21/21** targets compile. Focused **99.16s** passes 75 first-session and 10 restored
inputs. Final **391.16s** replay passes **1639 first-session inputs plus 10 new
restored-favour inputs**, both clean 151-preload boots, all 30 lifecycle checkpoints,
saved login/bank restoration and normal exits. Prior meaningful 1562-input coverage
remains, with positive mission/favour expectations and explicit cleanup. Two normal
purchases charge 500 once each; XP persists across updates, mission delivery still
works, and actual deadline expiry clears XP/object/cache. Restart preserves the
saved epoch/balance/member and applies without loading guild rooms. Same six
fixtures/two address overrides; no helper/grant, failed apply or worker panic.

The eager restore diagnostic's four-second limit remains unchanged; deferred
controller lookup removes the unnecessary room/NPC construction. Broad native
compiler performance is not claimed resolved. Next audit mission completion and
autonomous expiry; broader reward/stacking/cancellation policy stays separate.
The 957/1182 whole-source inventory remains its preceding sweep. Evidence:
/tmp/lima-favour-lifecycle-20261016.


Mission-expiry audit notes are committed in **cd678206**. The reviewed single
guild_d.c change now uses the body's existing name when present and 'system' for
playerless logs. Header/LF/final newline remain. No native API/code, master,
privilege, configuration, reward formula, clock or fixture change.

**22/22** complete targets compile. Final **369.75s** full replay passes
**1810 first-session inputs plus the retained 10 restored-favour inputs**, both
clean 151-preload boots, all 30 lifecycle checkpoints, saved login/bank restoration
and normal exits. Prior meaningful 1639-input coverage remains, adding **169
expiry inputs** before favour saved-state preparation. Real playerless STATE_D
expiry pays 500 for completed small wood/20 scraps and zero for incomplete/two
scraps, clears the controller, performs 10/5s cleanup and logs system outcomes.
Normal gifts and public duration/queue APIs exercise actual scheduler ending.
Favour restoration and all prior families pass; same six fixtures/two address
overrides, no new helper/grant, failed apply or worker panic.

Multiple-contributor/offline rewards, other recipes, overlap/repeat-end and broad
policy remain separate; cleanup explicitly flushes diagnostic logs, so autonomous
delayed-log persistence is not newly qualified. Next audit the remaining Yakitori
uniform, cashier/shop and delivery-boy consumers. The 957/1182 whole-source count
remains its preceding sweep. Evidence: /tmp/lima-mission-expiry-20261016.

Playerless guild logging is committed as **9e2f5a31** (native notes **b0274b98**).
The subsequent Yakitori uniform/cashier/shop/delivery-boy audit leaves a **five-file
temporary proposal**; dedicated LPC/native implementation remains unchanged.
Existing ARMOUR/armour-class and slashing/lightning damage names replace stale
uniform names, delivery boy retains one assistance inherit and the bundled std sake,
and cashier uses the existing exit-block callback API without changing privacy.
Conversation/assistance use native bound callbacks; conversation skips an empty
option addition. Headers/LF/CRLF/final newline and configured distinct nil/preserved
player/4000ms remain; no native API/master/privilege/registry/fixture changes.

**17/17** complete targets compile; **83.98s / 76-input** focused gameplay
passes 30 lifecycle checkpoints, two clean 151-preload boots, saved login and normal
exits, without failed apply/runtime input error/panic. Real uniform wear/remove and
coverage, cashier/kitchen constructors, outsider/member/solver exits, numbered
riddle/rejection, actual domain-gold skewer purchase and two equipped delivery boys
with sake pass. A one-shot helper-bound callback executes against a stock same-room
helper/flea under public target/health/special controls; natural player-attack hooks,
adjacent assistance and autonomous combat are not claimed. Same six fixtures/two
address overrides, no new helper/body/grant, controls/inventory restored.

Next adopt the reviewed proposal with the retained production replay, before
mission/expiry/favour preparation. Earlier 1810 first-session + 10 restored-favour
replay and 957/1182 whole-source sweep retain their measured scopes without replay
or additive count here. Broader original-game policy/native observations and
reserved inheritance/cache work stay separate. An earlier run passed all 75 gameplay
inputs but hit 4000ms during second-boot compilation; failure evidence remains and
no limit increase/native performance fix is claimed. Evidence:
/tmp/lima-yakitori-shop-audit-20261016.

Yakitori outfit/shop audit notes are committed in **7c07095e** (native **ab29ded1**).
The five source adapters are adopted with one indentation-only cleanup; all other
proposal bytes, headers/LF/CRLF/final newline remain. Uniform uses existing armour/
damage names, cashier supplies the public exit-block callback, delivery boy keeps
one assistance inherit and actual std sake, and concrete assistance/conversation
sites bind callbacks natively. Empty option additions are skipped. No native API/
source, master, privilege, config, registry or fixture change.

**37/37** complete targets compile. Full **413.0s** replay passes **1871
first-session inputs**, including **69 Yakitori inputs**, plus the retained
**10 restored-favour inputs**, all 30 lifecycle checkpoints, both clean 151-preload
boots, saved login/bank restoration and normal exits, without failed apply/runtime
input error/STATE_D error/worker panic. Prior 1810-input scope remains with unchanged
helpers and retained sequence comparisons; repeated observations and actual saved/
generated/menu values can vary. Real uniform wear/coverage, cashier conversations/
access/purchase, two equipped delivery boys and the scheduled one-shot same-room
assistance callback pass under public target/health/special controls. Natural attack
dispatch, adjacent assistance and full combat/AI retain their separate scope.

The new helper cleans up before retained mission/expiry/favour preparation and actual
restoration. Same six fixtures/two address overrides, distinct nil/preserved player/
4000ms; no new helper/body/grant. Earlier cold-preload timeout evidence remains; no
native performance fix is claimed. Next commit this adoption and refresh the source
inventory before selecting the next complete-object family. The 957/1182 sweep
retains its measured scope without additive estimates. Evidence:
/tmp/lima-yakitori-shop-20261016.

Yakitori support is committed as **65ce9796** (native notes **b03a0811**).
The subsequent inventory checks all **1182 sources**: **966 compile, 216 fail,
zero timeouts/crashes, 252.849s**. All **957** previous successes remain;
nine previous failures now compile, with unchanged source set. All **25 bundled
guild sources** and the artefact menu compile. This is an actual sweep, not an
additive estimate or completion percentage.

**61** failures are confirmed composed fragments, including the mission-control
module's newly confirmed real NPC composition. Changed conversation source is
revalidated with greeter/cashier hosts; prior classifications retain fresh host
passes. The other **155 failures** include unreviewed host parts and optional/
legacy sources, not 155 established complete-object bugs or native gaps.
Remaining first diagnostics/source locations match the prior sweep.

Next audit **std/slowheal.c** and six consumable examples: cigarette, joint, pain
killers and deluxe/large/small pet treats. All currently fail. Review numeric/
parser contracts and actual delayed healing before adapters, preserving dosage/
timing policy; no source proposal is made here. Raw source/pinned-original/line-
ending review is retained. Distinct nil/preserved player/4000ms and source/config/
binaries remain unchanged; this checkpoint changes notes only. No new gameplay or
Rust gate; preceding **1871 inputs + 10 restored-favour / 413.0s** full replay,
30 checkpoints/two clean 151-preload boots and saved login/bank retain their scopes.
Evidence: /tmp/lima-guild-source-inventory-20261016.

Inventory notes are committed as **92098d7c** (native **dab12c75**). The next
slow-healing/consumable audit leaves a **five-file temporary proposal**. Slowheal
uses explicit numeric conversion, an appropriate mixed parser guard, bound native
callback and an object predicate for active healing. A real bucket's missing flag
returns nil and otherwise falsely blocks the first item. Pain killers matches the
inherited parser argument; pet treats preserve integer FOOD values/multiplier and
deluxe weight1.0. Cigarette/joint need no edits. Raw headers/LF/CRLF/final newline,
healing/fuel/timing policy and distinct nil/preserved player/4000ms remain.

**27/27** complete targets compile. Focused **115.86s / 108-input** gameplay
passes all 30 lifecycle checkpoints, two clean 151-preload boots, saved login and
normal exits, without failed apply/runtime input error/panic. Real eat/light/smoke,
refusals, ordinary non-healer inventory, second-active guard, values and treat bites
pass. An actual natural 30s callback heals again and decrements9->8. Terminal checks
explicitly cancel pending timers and call public timed_healing(0); full natural
course/disconnect/transfer are not qualified. Controls and inventory restore.

Dedicated LPC/native source remains unchanged; only audit notes are pending.
Same six fixtures/two address overrides, no new helper/body/host/grant. Next adopt
with retained full replay before saved-favour preparation. Prior 1871 + 10 restored-
favour replay and 966/1182 inventory retain their scopes; no Rust gate or native
feature change. Evidence: /tmp/lima-consumables-audit-20261016.

Consumable audit notes are committed as **3dab1202** (native **026c4c82**).
The five LPC adapters now match the audited proposal exactly: numeric/parser fixes,
bound native timer and a truthy object predicate for healing flags. Original values,
fuel/timing/count policy and raw headers/LF/CRLF/final newline remain. No native API/
code/config/master/privilege/framework/fixture change; distinct nil/preserved player/
4000ms stay. Cigarette/joint need no source edits.

**62/62** complete targets compile. Full **470.4s / 1989 first-session
inputs**, including **108 consumable inputs**, plus **10 restored-favour inputs**
passes all 30 lifecycle checkpoints, both clean 151-preload boots, saved login/bank
and normal exits, without failed apply/runtime input error/STATE_D error/panic.
Prior 1871-input scope remains with unchanged helpers/retained comparisons. Actual
commands, guards, values/treat bites and a natural 30s healing callback pass. Terminal
checks explicitly cancel timers and call public timed_healing(0); the full course/
disconnect/transfer remains separate. Controls clean up before retained mission/
expiry/favour setup and restoration. Same six fixtures/two address overrides.

Implementation/updated notes are pending. Next audit camera/button/picture after
committing this adoption; inspect actual constructors/press/photo delivery. The
966/1182 sweep retains its measured scope without additive counts. No Rust gate or
native performance fix is claimed. Evidence: /tmp/lima-consumables-20261016.

Consumable support is committed as **109b22a0** (native notes **9c0eaf32**).
The next camera audit leaves a **one-file temporary button proposal**: explicit
mass conversion preserving SMALL=4 and a concrete press guard with default visibility/
short checks. Camera/picture and the existing callback stay. No native API/code,
config/master/privilege/framework/fixture change; headers/LF/final newline remain.

**11/11** complete targets compile. Focused **71.22s / 46 inputs** passes
all 30 checkpoints, two clean 151-preload boots, saved login and normal exits, without
failed apply/runtime input error/panic. Normal press/push creates four photos
(three carrier/one room), capturing two room descriptions and retaining old text
after movement. Get refuses button removal and preserves its child. In-memory
M_SAVE roundtrips preserve picture long and a single reconstructed camera button
that still presses; fresh-driver item restoration is not qualified.

Only audit notes are pending; dedicated LPC/native implementation stays unchanged.
Same six fixtures/two address overrides, distinct nil/preserved player/4000ms,
no new LPC helper/body/host/grant. Prior 1989 + 10 restored-favour replay and 966/1182
sweep retain their measured scopes. Next adopt with retained full replay after
consumables and before saved-favour setup. Evidence: /tmp/lima-camera-audit-20261016.

Camera audit notes are committed as **61122c8e** (native **3740cd3c**). The one
button-file change now exactly matches the audited proposal: explicit mass conversion
preserves 4, and a concrete press guard retains default visibility/short checks.
Camera/picture and the existing callback/get/do_not_restore behavior stay. No native
API/code/config/master/privilege/framework/fixture change; headers/LF/newline remain.

**72/72** complete targets compile. Full **413.38s / 2028 first-session
inputs**, including **50 camera inputs**, plus **10 restored-favour inputs**,
passes all 30 checkpoints, both clean 151-preload boots, saved login/bank and normal
exits, without failed apply/runtime input error/STATE_D failure/panic. Prior 1989-input
scope remains with unchanged helpers and retained comparisons. Four real press/push
photos deliver 3 to carrier/1 to room, preserve two room descriptions and old snapshots
after movement. In-memory M_SAVE picture/camera roundtrips preserve text and one
rebuilt button that still presses; fresh-driver item restoration remains separate.

Camera cleanup precedes retained mission/expiry/favour setup and actual restoration.
A retained first-run test failure came from randomized discarded-camera sentences;
the helper now compares six valid full snapshots differing only in that line, with
all other text exact. Focused 78.44s/50 inputs passes; source/RNG stay unchanged.
Same six fixtures/two address overrides, distinct nil/preserved player/4000ms; no new
LPC helper/body/host/grant. Implementation/updated notes are pending. Next audit the
writable-paper example/M_WRITING_SURFACE hook boundary after committing this change.
WIP burnable std/paper.c stays separate. The 966/1182 sweep retains measured scope;
no Rust gate or native performance fix. Evidence: /tmp/lima-camera-20261016.

Camera support is committed as **c9ec2f52** (native notes **d5fc3fb4**).
The writable-paper audit isolates a string/function hook boundary already present
in the reviewed upstream source. A one-file temporary M_WRITING_SURFACE proposal
stores one private nosave callback returning the current description and reuses its
identity for add/remove. The paper, existing write verb and shared hooks stay.
Original wording, 100-character paper, set_space/add_text policy and raw source
headers/LF/final newline remain; dedicated LPC is unchanged pending adoption.

**11/11** complete targets compile. Focused **71.26s / 61 inputs** passes both
write/inscribe forms, repeated writing, exact-fit and overflow checks without
mutation, stable paper-owned callback/unrelated stock callback coexistence,
actual look text and get/drop retention. All 30 checkpoints, both clean
151-preload boots, saved account login and normal exits pass without failed apply,
runtime input error or panic. This does not qualify written-item persistence across
restart; items are cleaned first. Same six fixtures/two address overrides and
actual master/simuls/user/body/parser; distinct nil/preserved player/4000ms stay.
No new LPC helper/body/host/grant or native change. Next adopt with retained full
replay after camera cleanup and before mission/expiry/favour setup. Prior
2028 + 10 restored-favour / 413.38s replay and 966/1182 sweep retain measured scope.
The separate WIP burnable std/paper.c remains outside this audit. Evidence:
/tmp/lima-writing-audit-20261016.

Writable-paper audit notes are committed as **0034b8dd** (native **31b43d61**).
The one M_WRITING_SURFACE change now exactly matches the audited proposal: a private
nosave callback returns current text and retains function identity for add/remove;
hook declarations match the host. Paper/verb/shared hooks stay unchanged, as do
headers/LF/final newline, original wording and capacity/add_text policy.

**75/75** complete targets compile. Full **426.29s / 2082 first-session
inputs**, including **60 paper inputs**, plus **10 restored-favour inputs**,
passes all 30 checkpoints, both clean 151-preload boots, saved login/bank and normal
exits without failed apply/runtime input error/STATE_D failure/panic. Prior
2028-input scope remains with unchanged helpers and retained sequence comparison.
Both write/inscribe forms, accumulated/exact-fit/overflow text, stable paper-owned
callback/unrelated callback coexistence, actual look and get/drop retention pass.
Fresh clones start blank; written-item restart persistence is not qualified.

Cleanup precedes retained mission/expiry/favour setup and actual restoration. Only
one unused informational print was removed from the audited helper; all assertions
and actions remain. Same six fixtures/two address overrides and actual master/
simuls/user/body/parser; distinct nil/preserved player/4000ms stay. No native API/
code/config/master/privilege/framework/fixture change or new LPC helper/body/host/
grant. Implementation/updated notes are pending. Next audit the physical small
mailbox/leaflet room props, separate from the completed user-mail system. The
966/1182 sweep retains measured scope; no native performance fix is claimed.
The retained first replay hit the existing 4000ms cold-load budget before paper;
source/helpers/limits stayed unchanged for the retry.
Evidence: /tmp/lima-writing-20261016.

Paper support is committed as **0bceb674** (native notes **99d7faf3**). The new
physical mailbox/leaflet audit is separate from the user-mail system. A two-file
temporary proposal explicitly converts their mass constants, preserving mailbox
16.0, leaflet 1.0, integer capacity 4, anchored refusal, IDs/text and relative
leaflet population. Dedicated LPC stays unchanged pending adoption; headers/LF/
final newline, native API/code/config/master/privilege/framework/fixtures remain.

**14/14** complete targets compile. Focused **69.49s / 62 inputs** uses the actual
entrance-room constructor and passes closed visibility/read/get refusal, normal
open/close/repeat/open-up, look-at contents, read auto-acquisition/full text/three
IDs, get/put/drop and anchored refusal. Four unit-mass leaflets fit and a normal
fifth put refuses without mutation. Explicit public reset calls replace a missing
leaflet once without duplicating an existing one; natural reset timing is untested.
Stock fillers/one temporary ID are instance controls, with no new LPC helper/body/
host/grant. The inherited generic look-in parser refusal stays unchanged; public
look_in hooks and ordinary open/look-at content display pass. The retained initial
failure was an incorrect test expectation about that refusal's wording.

All 30 checkpoints, both clean 151-preload boots, saved login and normal exits pass
without failed apply/runtime input error/STATE_D failure/panic. The room prop is
restored to one closed mailbox/leaflet; empty-handed player returns to Grand Hall.
Prop restart persistence is not qualified. Same six fixtures/two address overrides,
distinct nil/preserved player/4000ms and actual master/simuls/user/body/parser remain.
Prior 2082 + 10 restored-favour / 426.29s replay and 966/1182 sweep keep measured scope.
Next adopt with retained full replay after writing and before mission/expiry/favour.
Evidence: /tmp/lima-mailbox-props-audit-20261016.

Other first blockers include typed nil/float/value adaptations, callbacks, override
signatures and containing-object methods. A standalone failure is not automatically
an upstream bug or a missing efun. The ranged/ammunition batch adapts ten source
files, including explicit collector/default modes, a protected short-description
helper, nil sentinels, float values and native callbacks. Fire has its missing plural
action and falls back to the target's existing kill-permission check. Bounded source
checks pass **36/36**, including all 11 family targets and 17 tracked core objects.
The whole-source count above remains the earlier inventory.

The new normal-game run retains all 482 prior first-session command records and
adds **114 ranged inputs**. Nine real clones verify descriptions/configuration;
matching pistol/clip flows pass loading, readiness, mode changes and refusal cases,
15 shots, two clip depletions, removal and reload. A real quarrel callback verifies
bound-target movement and nil weapon-link cleanup. All 30 lifecycle checkpoints,
two 151-preload boots and saved login pass with normal exits. No new fixture source
or master grant is used. Crossbow/quarrel configuration, continuous combat, AI,
burst semantics, other source families and optional services retain separate scopes.

The retained production-driver check passes 30 checkpoints covering account/character
creation, menu, Grand Hall entry, look/inventory, logout and saved-account/body
restoration in a new process. Both processes exit normally. Cold boot and
saved-account restart each load 151 preloads with zero failures, satisfying this
configured lifecycle checkpoint. RST cleanup uses its existing unguarded helper;
native mkdir accounts for directories removed earlier in the same transaction.
The restart regression removes two seeded obsolete files and two nested output
directories, then recreates all seven RST categories. Only the disposable IMUD
router/OOB addresses use loopback. The original lifecycle check uses a receive-only
endpoint; the iftp check below adds a fixed mud-list response without establishing
a live I3 session. Existing wear/wield, loadout, armour and equipment-restoration
audits remain completed. Source-wide migration remains incomplete.

Todo passes inline submission with literal redirection text, editor completion
and abort; two actual news posts survive restart. Netstat passes ordinary/ignored
arguments and plain redirected output, and works again after saved login. Both
use protected CMD entry and unchanged shared implementations. Netstat counts an
empty socket list explicitly, preserving the driver's array identity semantics.

`socketinfo` uses native query_sockets mappings and a command-specific master
query grant. It reports native handles, states, stream/listener kinds and numeric
address/port pairs, with `-` for unknown endpoints. Six shell inputs and two after
restart verify the actual loopback stream/listener and wrapper metadata, ignored
arguments/input and the existing direct-output route (redirection creates an empty
file). Each report row is written separately within the driver's string limit.
All earlier login/menu/report checks remain; both 151-preload boots are clean.

`treefor` uses native syntax_tree to inspect a checked statement-body wrapper
without running it or creating a scratch object/source file. It prints escaped
lines directly, with usage and compile-error recovery. Eleven first-session and
three saved-login inputs cover arithmetic, captures, spreads, literal controls,
errors/retry and a 24,033-byte report containing all 100 locals. Existing account,
menu/report and lifecycle checks remain. Supplied code keeps normal source/dependency
permissions. The command's output format is native frontend diagnostics rather than
FluffOS compiler-node arrays; codefor remains the separate disassembly tool.

Guild catalog/governance loading and native persistence now work in the selected
distinct-nil configuration. Four stock mission definitions and one favour load;
class data, governance reports/menu branches, leader/manager changes and saved
restart pass. Ten known absent array values in the bundled guild save use explicit
nil; every other original byte and actual integer is preserved by that adaptation.
The catalog retains its existing nosave declaration and rebuilds on restart. No
native zero-import rule or class validation changes. Full mission/buff execution,
guild rooms/NPCs remain separate from this foundation.

All four guild special commands now compile through their shared SPECIAL base.
Its single function-exit combat callback uses a local catch/cleanup/rethrow wrapper,
retaining normal, early and error exits; no generic defer API is added. Controlled
command-pipeline cases check cleanup order, nesting and error propagation, and the
actual roundhouse callback retains its two-second delay. Registered shell checks
exercise the four command gates. These checks do not qualify full special-combat
gameplay or change existing targeting/reflex policy.

Unittest's command and shared suite framework now support native class fields,
callback values and nil object results. All five bundled suites compile after
38 callback registrations are converted; their test bodies are unchanged. Suites
remove themselves at the test daemon's request so reruns get fresh objects under
the existing master policy. A focused suite verifies callback arguments, expected
pass/fail counts, normal/debug output, quiet execution through the daemon API,
error propagation, fresh loads and restart. Full bundled gameplay assertions and
the separate test dummy remain unqualified by these framework checks.

Iftp's command selection and modal client now use native entry, nil and callback
forms. A fixed loopback mud-list response exercises real completion/canonical
lookup, help, directory handling, usage errors, all exit aliases, client removal
and saved-account restart. The actual IMUD daemon and client are used. This
qualifies the local client flow; live I3 file transfers/listing and alternate
configurations retain their separate scope.

Tasktool's actual menu and task daemon now support native callbacks, constructor
signatures, prompt construction and nil values. Registered input verifies task
creation, three-level navigation, completion rules, first/middle/last removal,
parent/root exits and menu cleanup. Nested removal preserves the full parent ID;
creation time is typed as an integer. Two saved root tasks and a completed child
restore through the real menu after restart. Admin-tool compositions and task APIs
outside the menu retain their separate scope.

Admtool's root uses native callbacks/nil metadata and builds layout only for
clones, so category discovery leaves unopened controllers unloaded. All ten
daemon menus now open, render help and return to their parents. Shared two-argument
prompts and alias input resumption work; selected group/alias edits persist through
restart, and colour/domain/method/channel reports run against real daemons. Global
alias defaults retain their existing shell-creation snapshot behavior, so the saved
alias becomes usable after a fresh login. Fixed loopback channel metadata supplies
nonempty I3 reports without a live router. All 25 leaf controllers now open as
described above; untested administrative actions retain their scope. No menu or
daemon is stubbed.

The focused runtime counts below retain their original scopes. Mudlist passes
13 cases using both actual shells; `su` passes refresh, race-change and password-
modal checks, with its account/character lookup limitation recorded separately.
Wizard directory navigation passes seven cases covering 28 inputs, including
saved directory history and current-file restoration after shell recreation.
Its root-origin `cd -` edge remains recorded in the port notes.

`ls` passes eight cases covering 29 shell inputs and two external calls. Checks
cover flags, metadata and loaded markers, globbing, plain/ANSI output, pipelines
and redirection; all 9 listing support targets compile. Its existing KB arithmetic
and stored plain-mode behavior are recorded in the port notes.

`cat` passes nine cases covering 34 shell inputs, including controls for nil EOF
and absent stdin. File contents, read boundaries, incoming pipelines, stdin
precedence and output redirection pass; all 10 file-output support targets compile.
Fresh transitional checks add only `cat`, with no regressions. Its existing
chunk-cursor omissions and missing current-file fallback are recorded separately.

`head` and `tail` pass eight cases covering 57 shell inputs. Checks cover file
selection, counts and flags, text edges, stdin precedence and head's outgoing
pipelines and redirection; all 12 supporting targets compile. Fresh transitional
checks add only `head` and `tail`, with no regressions. Tail's existing direct
pager output bypasses the outgoing pipe/redirection buffer; head's snippet
concatenation and count behavior are also preserved and recorded in the notes.

`grep` passes eight shell scenarios across 57 inputs. A separate range/glob
check confirms escaped pairs remain intact. File selection, flags, numbered and
inverse matches, stdin precedence,
Unicode and blank lines, slash patterns, pipelines and redirection pass. It uses
the existing editor module and reports its 8,192 UTF-8 byte, 1,000-line and
2,048-byte line startup limits per file, then continues. The driver's slice-read
fix preserves escaped pattern pairs. Inverse directory searches and existing
glob-pattern semantics remain recorded source limitations. Fresh transitional
checks add only `grep`, with no regressions; all 16 supporting targets compile.

`more` passes nine scenarios across 56 inputs through the registered user/pager
callbacks. File and stdin display, navigation, searches, help, shell escapes,
multiple files, completion and error cleanup pass; all 12 supporting targets
compile. Fresh transitional checks add only `more`, with no regressions. Its
existing direct-output routing, 999-line file cap, display spacing and unsupported
`-c` option are recorded in the port notes.

`cp` and `mv` pass ten scenarios across 61 registered shell inputs. Checks verify
exact file contents, source retention/removal, paths, empty/Unicode/NUL text,
overwrite/force, directory/glob selection, refusals and status output routes.
All 12 supporting targets compile; fresh transitional checks add only `cp` and
`mv`, with no regressions. Their existing text-based I/O, unused `-r` option and
same-file behavior are recorded in the port notes.

`mkdir`, `rmdir` and `rm` pass twelve scenarios across 42 registered inputs.
They use native command dispatch and preserve existing status messages and
argument rules. `mkdir` receives the grouped path array with an explicit
`mixed *` argument type; mkdir/rmdir refusals are caught per path so later paths
still run. Native `rmdir` accounts for earlier pending child removals and refuses
duplicate removal. Fresh transitional checks add only these three commands,
with no regressions; all 12 supporting targets compile. The existing parent/
child argument-expansion behavior is recorded in the port notes.

`find` passes nine scenarios across 47 registered shell inputs. It uses native
command dispatch and the existing editor module, reporting editor startup
refusals per file before continuing. The editor's 8,192 UTF-8 byte, 1,000-line
and 2,048-byte line limits remain. Fresh transitional checks add only `find`,
with no regressions; all 15 supporting targets compile. Its existing raw/case
pattern differences, file-only rule, ignored stdin and slash/empty-file
diagnostics are recorded in the port notes.

`locate` passes twelve scenarios across 61 registered inputs and 17 timer
callbacks. It searches filename records with native regex matching, rebuilds
in 50-directory batches and publishes the temporary index in a separate
callback after its writes commit. The complete copied tree produces 1,845
entries / 45,116 bytes; searches, pipelines, redirection and pager return pass.
First-batch and empty-tree publication, repeated rebuilds and file-error recovery
pass. Results retain the driver's 8,192-byte string-concatenation limit; oversized
output reports a diagnostic without partial results. Fresh transitional checks
add only `locate`, with no regressions; all 16 supporting targets compile.

`replace` passes twelve scenarios across 52 registered inputs. It uses native
command dispatch and the existing editor module, reports thrown per-file
refusals and closes retained editor sessions before continuing. Exact file
contents, patterns/captures, flags, pipes, redirection and pager return pass.
Fresh transitional checks add only `replace`, with no regressions; all 17
supporting targets compile. Existing last-file/duplicated reports and ignored
substitution diagnostics are retained and recorded separately in the port notes.

`cut` passes ten scenarios across 54 registered inputs. It uses native command
dispatch and the existing regex association simul. Field selection, delimiters,
Unicode, file expansion, stdin, pipelines, redirection and pager return pass.
Fresh transitional checks add only `cut`, with no regressions; all 19 supporting
targets compile. Its existing final-field omission, extra newlines/separators
and direct diagnostics remain recorded in the port notes.

`diff` passes eleven scenarios across 40 registered inputs. It uses native
command dispatch and the read-policy signature, and checks empty difference
arrays explicitly. Line comparison, paths, text edges, diagnostics, output
routes and pager return pass; all 22 supporting targets compile. Fresh
transitional checks add only `diff`, with no regressions. Its existing line
membership algorithm, raw filename parsing and direct pager output remain
recorded in the port notes.

`du` passes nine scenarios across 32 registered inputs and four direct helper
calls. It uses native command dispatch with the existing directory rule.
Directory totals and order, paths, output routes and pager return pass; all
25 supporting targets compile. Fresh transitional checks add only `du`, with
no regressions. Its existing KB arithmetic, recursion cutoff and blank root
display label remain recorded in the port notes.

`addpath`, `rmpath` and `printpath` retain all ten audited scenarios across
55 registered shell inputs. They use native command dispatch and `reduce`
for callback formatting, with explicit integer zero for empty `printpath`.
Cached and saved paths, ordinary lookup order, real shell reload, pipes and
redirection pass. Fresh transitional checks add only these three commands,
with no regressions. Existing directory rules, empty-rmpath formatting and
ignored arguments/stdin remain recorded in the port notes.

`xargs` retains all twelve audited scenarios across 40 registered inputs.
It uses native command dispatch, bound callbacks and explicit object/callback
arguments. Per-line piping, both placeholders, empty stdin, modal clone reuse,
exits, shell escape and context/recovery pass. Fresh transitional checks add
only `xargs`, with no regressions. Its existing option-spacing mismatch,
dropped lock argument and separate child-output routing remain recorded in
the port notes.

`do` retains all thirteen audited scenarios across 31 registered inputs.
It uses native command dispatch and a bare return after its syntax report.
Ordered and nested commands, player/directory context, failure recovery and
modal interaction pass. Fresh transitional checks add only `do`, with no
regressions. Literal semicolon splitting, separate child stdout and ignored
stdin remain recorded in the port notes.

`times` retains all fifteen audited scenarios across 46 registered inputs.
Its dispatcher forwards original input and parsed arguments to the existing
handler. Counts and administrator login, nested commands, player/directory
context, exception recovery and repeated modal entry/exit pass. Fresh
transitional checks add only `times`, with no regressions. Existing numeric
prefixes, word joining and ignored outer routes/stdin remain in the port notes.

`at` retains all fourteen audited scenarios across 39 registered inputs.
Its dispatcher forwards parsed arguments to the existing handler. Temporary
movement and normal return, target diagnostics, player/directory context,
move refusals, child errors, output routes and modal input/location/cleanup
pass. Fresh transitional checks add only `at`, with no regressions. Existing
ignored move results, skipped return after a child exception, target lookup,
separate child stdout and ignored stdin remain recorded in the port notes.

`force` retains all fourteen audited scenarios across 50 registered inputs.
Its dispatcher forwards raw input to the existing handler. Both actual shells,
target context, notices and logging, nested commands, caller error recovery,
target modal interaction and child output routes pass. Fresh transitional
checks add only `force`, with no regressions. Existing literal account-name
lookup and ignored parent stdin remain recorded in the port notes.

`to` retains all fourteen audited scenarios across 53 registered inputs.
Its dispatcher and bound callback execute the chosen command after text-editor
completion. Abort, empty completion, arguments/context, child errors, chained
editors, repeated binding and stack teardown pass with cleanup intact. Fresh
transitional checks add only `to`, with no regressions. Existing parent stdin
and delayed child output routes remain recorded in the port notes.

`bk` and `rst` retain all eighteen audited scenarios across 64 registered
inputs. Dispatchers make both commands available; bound restore callbacks
handle selection, retries and cancellation. File errors use the existing
diagnostics and cleanup. Backup bytes, empty files, current-file state and
output routes pass. Existing absolute-current-file paths, missing backup
subdirectories and final collision overwrite remain recorded in the port notes.

`update` passes 34 scenarios across 68 registered inputs, including retained
controls and clock/error fixtures. Reloads replace prototype identity and state
while old clones keep their compiled program. Flags reload loaded parent
prototypes; timestamps include unloaded inherited sources without instantiating
fragments. Successful room reloads and skips return players. Failures retain
the existing early cleanup and caught errors; the same-second timestamp edge
remains documented.
Fresh transitional checks add only `update`, with no regressions.

`renew` passes 23 scenarios across 54 registered inputs, including baseline
and minimal-port controls. It uses native command dispatch and a bare void return.
The destruction simul handles contents after cleanup: children try to move to the
parent environment, and those remaining are retired recursively. Inventory
snapshots are processed in batches. Saved types, replacement identity/state,
nested contents, movement outcomes, errors, output routes and shell context pass.
Existing duplication, reconstructed relation bookkeeping and buffered helper
output remain recorded in the port notes. Fresh transitional checks add only
`renew`, with no regressions; native destruction and permission applies are
unchanged.

`loadall` passes all seventeen audited scenarios across 25 registered inputs,
including retained controls. Its dispatcher exposes the private handler; explicit
load checks stop later replacements after a failed load. A resident master/simul
preflight runs before cleanup. Batch removal, old clones, original callbacks,
self-exclusion, contents cleanup, errors and output/context behavior remain.
Fresh transitional checks add only `loadall`, with no regressions.

`stat` passes all thirteen audited scenarios across 27 registered inputs,
including retained controls. Its dispatcher exposes the private handler; the
body passes the bare `our_description` callback so `stat me` reports its real
description. Paths, clones, inventory IDs, inherited reports, absent information,
errors, output routes and caller/player context retain their audited behavior.
Fresh transitional checks add only `stat`, with no regressions.

`spew` passes all nine audited scenarios across 28 registered inputs, including
retained controls. Its dispatcher exposes the private handler; selected lines
use the existing say verb, recipient delivery and speech history. Paths, ranges,
text, read errors and retry, output routes and shell/input context pass. Fresh
transitional checks add only `spew`, with no regressions. Existing partial
numeric scans, whitespace-only parser diagnostics and empty outgoing output
buffers remain recorded in the port notes.

`whoguests` retains all seven audited scenarios across 18 registered inputs,
including the unavailable-command control. Its dispatcher exposes the private
handler; sorted reporting leaves guest order, membership and saved bytes intact.
Empty lists, daemon add/remove/clear, saved-data reload, extra arguments, ordinary
administrator checks and retry, output routes and shell/input context pass. Fresh
transitional checks add only `whoguests`, with no regressions. Existing ignored
command text and raw no-redirection suffixes remain recorded in the port notes.

`tsh` retains all eleven audited scenarios across 28 registered inputs,
including the unavailable-command control. Its dispatcher exposes the private
handler; batch lines use the actual wizard shell and current user/body context.
Nested batches, paths, directory changes, blank lines, Unicode, CRLF, errors and
retry, output routes and modal entry/exit pass. Child output keeps its own routes;
the parent sends an empty output buffer and ignores incoming stdin. A child
exception stops later lines while keeping completed effects. Fresh transitional
checks add only `tsh`, with no regressions. Existing bottom-shell dispatch while
a modal handler is open remains recorded in the port notes.

`testargs` retains all ten audited scenarios across 43 registered inputs,
including unavailable dispatch and the old-callback control. Its dispatcher,
explicit file_name argument and bound callback support interactive input,
both exits, clone reuse/cleanup, shell escapes, directory changes and recovery
after child errors. Parent output routes remain empty while child output reaches
the user. Existing prefix truncation, stdin argument-type errors and the
missing-command unused clone remain recorded as Lima source problems. Fresh
transitional checks add only `testargs`, with no regressions.

`dataedit` retains all eight audited entry scenarios across 25 registered inputs,
including unavailable dispatch, the uncompiled editor and terminal-split-field
controls. Command dispatch, declaration visibility, typed nil and assignment
repairs, native value codec names, function callbacks and local split boundary
normalization let native saved files open in the real menu. Discard preserves
bytes; save/reload preserves zero, nil, floats, text, containers and nominal class
records. Both quit paths remove the handler and retire the active clone. Empty
and comment-only files, argument handling and relative quoted paths retain shell
context. Existing reader errors leave unused clones but allow valid retry.
Fresh transitional checks add only `dataedit`, with no regressions.

The editing-actions follow-up retains fourteen cases across 105 registered
inputs, including one original-source merge control. Merge dispatch uses
`mappingp` and `arrayp` because native `typeof()` returns integer tags; arrays
and mappings now combine. Scalar diagnostics, false-value replacement and
mismatch behavior remain. Delete, rename, set, print, listing and save-as retain
values, prompt ownership and shell context, with native save/reload and
checkpoint/discard checks. Existing empty-list failure, expression artifacts
and generic feedback on errors, and save-as I/O failure remain recorded; valid
retries keep the menu usable. Concurrent editors and commit-time delivery
failures were not exercised. Fresh transitional and support compilation checks
retain the preceding inventory without regressions.

`docs` uses the command dispatcher, explicit void returns, local path-split
normalization and integer/array defaults for category counts and absent RST tags.
Fourteen retained cases across 23 registered inputs check reports, deduplication,
category counts, sorted ten-file topic lists, and actual ASCII/Unicode/simplified
widgets. Administrator player-help generation uses real command/verb discovery,
handles empty verbs and overwrites the page deterministically. Refresh runs the
existing help daemon, including flat/nested completion, its busy guard, native
saved data, scan failure with saved-byte preservation, and successful retry.
The help daemon, write rules and native APIs remain unchanged. Full help-tree
scanning and commit-time delivery failures retain their preceding scope.

`codefor` compiles its expression in memory, obtains native instruction lines
with `disassemble`, and prints them individually without the command output
buffer or a pager. It does not invoke `eval_function` or write/remove EVAL.c or
an assembly dump file. The temporary wizard-home CODEFOR object is destructed
after output or a listing failure; an occupied name is reported and retained.
Usage and compiler errors leave the shell ready for another command.
Native `compile_string` authorization and initialization still apply.

`dbxfuncs` inspects loaded objects using native `functions` and
`function_signature`, preserving return types, fixed-parameter placeholders and
defining programs. Results include inherited public functions and local
overrides; foreign private/protected methods stay hidden. Signature counts
exclude variadic collectors. Usage, missing objects and no matches are reported
through command stdout, along with headings and rows, so pipelines and output
redirection receive the complete report. Regex errors leave input usable.

`dbxvars` reports matching globals in loaded objects through native
`variable_info`, including inherited, private and non-persistent declarations.
The master preserves the original binding permission rule: inspection requires
the target's write privilege through Lima's existing security daemon. This check
runs before filtering, including when the pattern would match no variables.
Each shadowed declaration reports its own value with the declaring source in
its label. Strings and nested string values remain quoted. Headings, values and
usage messages share command stdout for pipelines and redirection; missing
objects, permission failures and regex errors leave shell input usable.

`dbxwhere` displays saved native error diagnostics for the current account, a
named account, or `last`. Reports include a timestamp and the stored native
diagnostic, with source excerpts and stack traces when available. Command stdout
supports pipelines and redirection.
Saved named records remain available without a live player body. Empty records
and unknown players retain their existing messages. These records come from the
master's uncaught-error handler and the user input layer's caught-message recorder.
Caught command and modal failures update the current account and `last`, with
wizard detail or the usual generic player bug notice. Input resumes even if the
recorder fails. Internally handled errors and successful input preserve history.
The recorder attempts to append messages to `/log/catch`.

Caught runtime records retain only the message; propagated compilation errors
retain their native source diagnostic. Uncaught records retain the native rendered
stack when available. With native saved-frame support, `dbxframe` displays an
uncaught record's named arguments and locals as rendered diagnostic text, with
source excerpts, defining program and receiver names. Frame numbers index the
retained root-to-leaf list from zero. Container values use summaries, and omission
counts and truncation flags identify shortened snapshots. Current-account, named
account and `last` lookups, pipelines and redirection are supported.

Caught errors explicitly report that frame data is unavailable. Missing, empty or
changed source files do not replace the saved error, and source excerpts use the
recorded source file rather than its defining program. Large displayed reports
are capped at 1,600 Unicode characters plus a truncation notice to fit command
stdio's single-string buffer; this does not alter the saved snapshot.

`checkpriv` reports whether the current command and user context has a privilege,
using Lima's existing validation and privilege checks. Exact `0` and `1` inputs
select integer privilege levels; registered names and suffixes remain strings.
Blank input shows usage. Usage, verified/denied results and invalid-privilege
feedback share command stdout for pipelines and redirection. Shell argument
quotes remain literal.

`snoop` keeps Lima's optional target and reason-word rule. Wizards require the
target's `snoopable` flag; administrators retain their override. Successful
subscriptions log account names and the reason, using `no reason given` when
omitted. Feedback shares command stdout for pipelines.

The daemon invokes its cycle-check callback directly, permitting ordinary
chains while rejecting self- and back-edge cycles. Each watched account has
one native relay shared by its subscribers. The existing callback forwards
buffered output and decoded input; subscribers can detach independently,
and a stopped relationship can be started again.
`unsnoop <user>` removes one subscription; `unsnoop` removes all targets for
the current body. Feedback shares command stdout for pipelines and redirection.
Removing the last subscriber retires the relay. Reloading the snoop daemon
cleans up its subscriptions and relays before loading the replacement, so
subscriptions can be started again afterward.

The administrator command `snoops` lists subscribers and targets in name order,
using command stdout for pipelines and redirection. Destroyed target references
are omitted from the display.

The administrator command `listeners` lists a named channel or all channels,
including channel flags and linked, link-dead and non-body listener formatting.
Destroyed references and nil entries are omitted from the display while the
stored channel membership remains unchanged. Output supports pipelines and
redirection; the existing object-argument report remains unfinished.

`addguest <name>` and `removeguest <name>` maintain the saved guest list used
when `NO_NEW_PLAYERS` is enabled. They require an administrator and preserve
case-sensitive names and feedback for duplicate additions or missing removals.
Both commands keep direct user output and raw arguments: redirect and outgoing
pipe suffixes form part of the name, and incoming pipeline text is ignored.

`master` queues an in-place update of the resident master object; `master <id>`
reports its queued, succeeded or failed status. Administrator privileges are
required when requesting an update and when preparation starts. Successful
updates preserve object identity and compatible state without rerunning `create()`;
failed compilation or initialization leaves the old master usable. Status messages
support pipelines and redirection; incoming stdin does not supply a request ID.

`objpurge <base-name>` requires an administrator and removes loaded instances
of one exact extensionless filename, including its prototype and clones.
The selector is raw text; relative paths, extensions and quoted names do not
match. Objects receive `remove()` first; surviving objects receive it again
through the destruct wrapper before destruction. Cleanup failures return to
the shell with preceding hook writes retained. Counts support pipelines and
redirection; incoming stdin does not supply the selector. Omitting the filename
retains the command's caught formatting error.

`shutdown <reason>` requests immediate shutdown; `shutdown -t<minutes> <reason>`
starts a minute countdown and `shutdown -c <reason>` cancels it. Administrator
privileges are required by the command and rechecked by the final timer action.
The configured preserved call-out player supplies the initiating user context.
Announcements go directly to connected users; startup feedback uses command
stdout for pipelines and redirection. Incoming stdin does not supply the reason.
The command logs the request and quits users before calling the privileged
shutdown simul. The master accepts the native request only through that wrapper
with its existing privilege check.

The existing second-request path clears its cancellation handle while the
countdown remains scheduled, so a later cancellation reports no shutdown in
progress. This source issue remains separate from the command adaptation.

`wall <message>` requires an administrator, appends the broadcast to `/log/walls`
and delivers it to the sender and other connected users, including users without
a live body. It retains Lima's punctuation rules and each recipient's wrapping.
Empty input shows usage. Arguments stay raw: quotes and redirect or outgoing
pipe suffixes remain message text, and incoming pipeline data is ignored.

`telnet` requires an administrator and opens a modal client. Normal input stays
with the shell until `/tog` routes it to the active connection; `!` sends a line
to the shell, and `/send` sends explicitly. `/open` accepts an address or a case-insensitive
unique mud name, `/jobs` lists sessions, `/goto` selects one and `/close` closes
it. Output from inactive sessions is buffered; `/hide` toggles display and
`/exit` returns to the shell. The control prefix defaults to `/` and follows the
user's `telnet_char` setting. Incoming pipeline text does not supply an address.

The client uses the port's UTF-8 text socket wrapper. Telnet option negotiation
remains unfinished in the upstream client. Existing failed-open session records
and the receive loop's access to closed slots remain separate source issues.

`echo <text>` sends raw text to other bodies in the room and acknowledges the
sender. With an outgoing pipe, it supplies the raw text to command stdout.
`echoall <text>` sends normalized rule arguments to other attached users across
rooms, including users without bodies. `echoto <player> <text>` selects a linked
body and preserves raw message text, delivering only to that target.
`echom [@target] <action>` uses Lima's message and grammar modules for actor,
target and onlooker views, with body, carried-object and room-object lookup.

`echoall` disables IO, but its parsed arguments omit operator suffixes; this
retains the original shell's behavior. `echoto` keeps those suffixes as literal
message text. Direct messages from `echo` and `echom` bypass stdout, so permitted
file redirection creates an empty file. Existing `Mudlib:wizcmds` privilege can
deny home-directory output after message delivery. Incoming pipeline text does
not supply a message. Ordinary player shells exclude these wizard commands.

`emotem [@target] <action>` prepends the actor's name through Lima's message
syntax. It finds a body, carried object or room object as the target; an unknown
or bodyless target falls back to a simple room action. Self-targeting retains
the existing two direct sends. Its direct messages leave redirected stdout
empty, and incoming pipeline text does not supply an action.

`msg <type> <message>` adds a personal message variant using normalized rule
arguments. `msg <type> clear` removes the personal override, exposing a stock
default again when one exists; a custom type disappears. Its acknowledgement
uses normal stdout, file and pipe routing.

`review [-cd] [player]` displays every message variant, using your own view or
another linked body's onlooker view. The inspected player receives no output.
`-d` includes raw templates; `-c` is accepted but unused. The existing raw-template
display removes the last character even without a newline; rendered actions keep
their punctuation. Review uses normal stdout routing and ignores incoming pipe
text. Ordinary player shells exclude these wizard commands.

`vis` makes your body visible and `invis` hides it. Both use your personal
message template for a direct action to you and other bodies in the room,
keeping your name in both transition messages. Repeated calls show only status.
Arguments and incoming pipe or file input are ignored.

Normal status supports stdout, file and pipe routing. When you are already
visible, `vis` sends its status directly instead, leaving redirected output
empty. Existing command privilege can deny home output after action messages
and visibility changes. Ordinary player shells exclude these wizard commands.

`quiet <object>` stops periodic actions and clears the action list. Bodies cannot
be quieted; objects without an action array are already quiet. Queued responses
and existing movement/room hooks remain. A later move can schedule one idle
callback that does nothing and stops. Status uses normal stdout, file and pipe
routing without a newline; incoming input is ignored. Existing command privilege
can deny home output after action state changes. Ordinary player shells exclude
the command.

`lightme <number>` sets total body light, including visible carried light, and
updates the room by the difference. Room visibility messages remain direct;
status uses stdout, file or pipe routing with a newline. Moving a carried light
out after resetting total light can leave the body with negative light, as in
the existing setter. Numeric arguments retain integer-prefix parsing. Incoming
input is ignored, and ordinary player shells exclude the command.

`heal [player]` heals yourself with no argument, `me`, `myself` or your own
account. It resolves other connected bodies by account, lower-case display name
or nickname, including remote bodies. With the selected limb health system, it
restores damaged and disabled limbs, clears drunkenness and refills reflex points.
Action messages and lookup refusals remain direct, leaving redirected file/pipe
output empty. Incoming input is ignored. Existing command privilege can deny
home output after healing; ordinary player shells exclude the command.

`print <text>` preserves raw spacing and quotes and appends a newline. Incoming
file or pipe input takes precedence, even when empty, and passes through unchanged.
`null [argument]` discards its optional argument and incoming input and emits
nothing; it sends empty output to the next pipeline stage. Both use normal stdout,
file and pipe routing. Ordinary player shells exclude them, and existing command
privilege can deny home output.

Bare `print` without text or input still raises a type error. The shipped `@`
alias wraps its entire remainder in a backtick expression; use `print` with an
explicit backtick expression for redirected evaluation. Repeated evaluations
preserve player context and use your optional `eval.h` header.

`eval(text[, header])` converts canonical numeric text and evaluates longer LPC
expressions. Repeated calls preserve player context and support an optional
include header.
Evaluated strings, nil and other false results return the original text, as do
caught expression errors; `eval("0")` returns integer zero. The generated source
stays in `/tmp/eval.c` until the next call. The existing object-name shortcut
returns text; use `find_object()` to look up an object.

`which <command>` prints the first matching command in your shell's search
path. It accepts a command name, preserves missing/noncommand diagnostics, and
does not execute the command it finds. A matching noncommand file stops the
search. Output supports stdout, file and pipe routing; incoming input is ignored.

`calls` lists active timers grouped by owning object and callback name, followed
by the state daemon's queue. Named callbacks with different bound arguments share
a group. Delays use whole seconds; elapsed timers show a blank delay. The listing
does not run callbacks. Output supports stdout, files and pipes; extra arguments
and incoming input are ignored.

`date` displays local driver time. `date -s [text]` prints a source-comment
stamp with your body name, using `Created` when text is omitted; supplied quotes
and spacing are preserved. Its direct user messages stay on screen under file or
pipe redirection: overwrite produces an empty file and append preserves existing
contents. Incoming file or pipe input is ignored.

`cpu` reports the driver process's accumulated user and system CPU time as a
percentage of elapsed driver uptime. Milliseconds are retained, and parallel work
can exceed 100%. Zero uptime or unavailable CPU counters produces `CPU usage:
unavailable.`. Output supports stdout, files and pipes; arguments and incoming
input are ignored.

`status` displays raw integer process resource counters. User and system CPU
times are milliseconds, maximum resident set size is peak KiB, and integral-memory
fields use OS accounting units. Existing labels remain, with capitalized field
names for unlabelled counters. Unavailable resource data prints only the header
and delimiters.

The table goes directly to your screen, including under file or pipe redirection:
overwrite produces an empty file, append preserves existing contents, and the
next pipeline stage receives empty output. Arguments and incoming input are ignored.

`wc [-lw] <files>` counts words and lines, with globs and quoted filenames
supported. Supplied file or pipe input takes precedence over filenames, even
when empty. Counts use normal stdout, file and pipe routing; usage messages stay
on screen, so redirected usage produces empty output.

Line counting drops one empty field at each newline boundary, retains interior
empty fields, and returns zero for empty or newline-only text. Existing word
counting splits on spaces, tabs and newlines and includes empty boundary gaps:
an empty file reports one word, and a trailing newline adds an empty word gap.

`apropos <text>` searches help-topic names using case-sensitive literal
substrings. Results group names by category and sort category labels, omitting
the leading help directory. Each matching file contributes its topic name,
including duplicates. Quotes remain literal; the shell trims outer whitespace.

Reports support stdout, file, append and pipe routing; incoming input is ignored.
Usage, short-search and no-match messages stay on screen. Redirected usage or a
short search produces empty output; a no-match result still writes one newline
to the redirected file or next pipeline stage.

`flist [-i] <object>` lists resolved function names in a fixed 79-column table.
With `-i`, rows sort by defining program and then name. Inherited functions name
their defining program; overrides name the target program. Clones share their
program origins. Foreign objects expose public functions; self-inspection can
include private and protected functions. Object paths and `me`, `here` and
`shell` aliases use the normal object resolver.

Reports support stdout, file, append and pipe routing, with normal paging on
screen; incoming input is ignored. Reports are buffered and can exceed the
8,192-byte string-concatenation limit, including large `flist me` listings.
The shell catches the error and accepts the next command. Overwrite redirection
removes the previous file before report construction, so failure leaves it absent.

`users` lists connected user IDs in the order returned by the driver. Connections
still logging in display `(login)`; empty string IDs remain empty. When you are
the only connected user, it prints `You are the only person logged in!`.

Final output separates names with spaces, including file and append redirection.
Nonfinal pipeline output separates names with newlines, preserving empty names
and a final newline. Reports support normal paging. Arguments and incoming file
or pipe input are ignored.

`livings` counts attackable clones that are not player bodies, grouped by both
short description and base program path. Clones without an environment or in
rooms that forbid combat are omitted. Arguments and incoming input are ignored.

Reports go directly to the screen without paging. Under redirection, overwrite
writes an empty file, append retains existing text, and pipes receive no report.

`wheremobs` lists loaded clones matching a filename, excluding player bodies
and prototypes and omitting matches without an environment. Relative paths and
optional `.c` suffixes work. It includes ordinary clones and does not require
attackability. Existing source files take precedence over ID matching.

The original command resolves IDs as paths before matching names: at `/`, `rat`
seeks `/rat`, so a mob named `rat` is missed. This Lima source limitation remains.
Reports go directly to the screen without paging. Redirection overwrites with
empty output, append preserves existing text, and pipes receive no report.
Incoming file or pipe text is ignored.

`where` reports account IDs and their current bodies' room filenames and short
descriptions. With no targets it lists connected users that have bodies; named
targets retain their order and duplicates. Names may be separated by spaces or
commas. It reports saved accounts without an active connected body as offline
and unknown names as nonexistent. Character names do not replace account IDs.

Lookup remains case-sensitive. Noncanonical text gets the nonexistent-name row
without entering the ported registry's canonical-name API. Reports use command
output buffers, supporting overwrite, append, pipes and normal paging. Incoming
file or pipe text is ignored.

`here` reports the current body's room short description and filename, including
a clone suffix when the room is cloned. Shell working-directory changes do not
change the room being reported. Extra arguments and incoming file or pipe text
are ignored. Reports use command output buffers, supporting overwrite, append,
pipes and normal paging for multiline room descriptions.

`showexits` reports every exit direction from the current body's room, including
hidden exits, and loads ordinary destinations to obtain their short descriptions.
Failed loads display "Under construction"; nil short descriptions become empty
text. A missing destination is shown as None (0). Destinations beginning with `#`
are printed verbatim, including their own line endings. Extra arguments and
incoming file or pipe text are ignored. Reports use command output buffers,
supporting overwrite, append, pipes and paging.

`uncd` swaps the current and previous working directories in the wizard shell,
leaving the current working file and body unchanged. It takes no arguments and
does not check whether the previous directory still exists. When no previous
directory is recorded, it selects nil and reports NONE; a second call restores
the former directory. Incoming file or pipe text is ignored. Output uses command
buffers, supporting overwrite, append and pipes. Argument rejection and refusals
opening this command's input or output occur before the swap.

`scan [-d] [object]` lists the native inventory of an object, defaulting to the
current body's room. The `-d` flag traverses nested inventories. Reports retain
clone filenames, shorten /domains/ to ^ and /wiz/ to ~, quote short descriptions,
and use [no short] for nil descriptions; empty strings appear as "". Inventories
are visited in reverse native order; no upstream ordering guarantee is implied.
Output uses command buffers, supporting files, pipes and paging. Incoming file
or pipe text is ignored. Argument and stdio refusals preserve shell/body state.

Terminal wrapping preserves leading indentation when text fits the configured
width. Recognized colour markers do not count toward word widths. Right trimming,
word-boundary wrapping and continuation indentation retain the existing policy.
No-wrap, prompt, blob and simplified output keep their existing handling;
redirected command output retains its exact bytes.

`showtree [function] source` reports the loaded inheritance tree, or the branches
containing visible definitions of a bare function name. Shared ancestors appear
along each inheritance route. Private and protected definitions are hidden by
native function inspection. A missing function gives only the tracing heading
when all parents are loaded. Source names and object aliases resolve through the
wizard file rule; clone aliases select their prototype source.

Traversal uses existing objects and reports "Couldn't find object" for an
unloaded parent. Compiled inheritance alone does not create parent objects, so
such reports are partial; this command does not initialize parent programs.
Output uses command buffers for files, pipes and paging. Incoming file or pipe
text is ignored. Existing optional-argument and stdio refusals retain shell state.

`last [-s] [-n count] [-d days] [-D days] [users...]` displays saved login
history. Named users are matched after lowercasing; online status follows the
existing account-name lookup. Named lookups ignore the
history-list filters. List mode orders records by timestamp, excludes future
records by default and uses inclusive day bounds. Omitted or zero counts show
all records; a positive count selects the newest records. Existing leading-
integer conversion and negative-count behavior are retained.

The port supplies a zero default before converting an omitted count, tests user
presence by truthiness and sorts daemon records with an explicit timestamp
comparator. Output uses command buffers for files, pipes and paging; incoming
text is ignored. Login/logout registration and daemon persistence keep their
existing API and record format.

`people [flags]` displays connected bodies in frame columns. With no flags it
uses `people_flags`, or `anFwh` when that variable is unset. Flags are raw
characters rather than getopt options. Role filters retain their `W`, `A`, `P`
precedence; wizard-only columns, visible-body filtering for nonwizard callers,
and the direct menu-entry function retain their existing handling.

The port supplies protected command dispatch, uses nil for absent string
arguments and passes an environment filename to `filename_ellipsis`. `bodies()`
already returns live body objects, so no zero subtraction is needed. Shared
frame layout, column selection by width, headers and footer state remain as
implemented by Lima. Output uses command buffers for files, pipes and paging;
incoming text is ignored.

`whoip [account-or-address]` displays connected users and saved login-address
history. With no argument it includes connections still at login. Online
account lookup displays everyone sharing that connection address, then that
account's stored addresses. Address lookup displays matching connected users
and the saved account list for the exact address key.

The port supplies protected dispatch for the existing raw-string command rule.
Names and history keys retain their case-sensitive handling; the existing
numeric-prefix/hostname branches and report wording remain. The history daemon
keeps its current data, registration and persistence API. Output uses command
buffers for files, pipes and paging; incoming text is ignored. Native connection
metadata is displayed as provided by the existing inspection efuns.

`goto <object>` keeps the existing wizard object rule: room filenames resolve
through the shell directory, while a player or item selects its environment.
`me` and `here` retain their shortcut meanings. The port adds protected dispatch
and retains body movement, same-room twitching, force-look behavior and direct
string refusal messages.

`home [wizard-name]` keeps its raw-string rule. With no name it uses the body's
permanent workroom or a cached temporary workroom; named visits require the
existing workroom file. Brief mode suppresses its look command. The earlier
`clone_object` adaptation remains. Both commands use existing output buffers
and ignore incoming text. Body messages and room descriptions retain their
ordinary delivery path. Existing `home` behavior does not check a failed move
before continuing its messages and conditional look; the port retains that
behavior.

`trans <object>` uses the existing wizard object rule and moves a target into
the caller body's room. Self and already-present targets retain their current
messages; a failed move reports the existing generic refusal. The target sees
its private transport message and forced look, while room broadcasts keep their
current recipient exclusions and `Someone` suppression.

The port adds protected dispatch. Linked targets supply their user as the
logical player for `force_look`; unlinked targets supply the target itself.
The earlier catch/restore/rethrow adaptation restores the caller's player on
success and look failure. Command buffers support files and pipes; incoming
text is ignored, and target/body messages retain ordinary delivery. Current
name-based self checks and the placement of the short-name fallback remain.

`wizz [mount-text]` returns the caller to the wizard lounge. It keeps the raw
string command rule: supplied text, including quotes, is used as the mount
description; omitted text selects an existing random description. Already being in the
lounge produces the existing twitch; movement failure reports the generic
refusal without a look or arrival messages.

The port adds protected dispatch. Successful movement retains forced look,
caller confirmation, origin/destination broadcasts and exact `Someone`
suppression. Command output supports files and pipes, while body/room messages
retain ordinary delivery; incoming text is ignored. A failed forced look leaves
the completed move intact and prevents the later confirmation and broadcasts.

`Move <object> <object>` keeps the existing two-object wizard rule and adds
protected dispatch. Object aliases, held objects, players and object paths retain
the existing resolver. The command calls the source's ordinary `move` method,
then tells it the destination description and broadcasts into the destination,
excluding the source from that broadcast's contents delivery. Body movement does
not force a look. Caller shell state and directory remain independent of movement.

The current command ignores a refused move and continues its transport messages;
that upstream behavior is retained. Messages use ordinary target/container
delivery rather than command buffers, so redirected command output is empty and
incoming file or pipe text is ignored. A target-message error occurs after the
move and stops the later broadcast under the existing shell error handling.

`clone [source-file]` keeps the optional compiled-file rule and current-file
fallback. The earlier `clone_object` adaptation remains; the port adds protected
dispatch and uses `deep_inherit_list` for the fallback's standard-object ancestry
check. That check requires an ancestor and excludes the base program itself.

Fresh copies run `create` before placement, then the command calls `on_clone`
after inventory placement, room fallback or a failed placement. Gettable copies
prefer inventory; nongettable copies and inventory refusals fall back to the room.
A failed move to both destinations retains an unplaced copy. The current-file
fallback destroys a copy without the required ancestry and reports usage. If
existing master policy refuses that cleanup, the error leaves an unplaced copy.

Status text uses command buffers for files and pipes; player messages retain
ordinary delivery. Incoming text is ignored. Initialization failure rolls back
the failed copy, while a later callback error retains its completed placement
and prevents flushing the buffered status. With no filename or selected current
file, the existing filename-argument error is retained.

`dest object` retains the general object rule. The port adds protected dispatch.
The command calls `do_remove`, sends its ordinary player-message or nil-short
filename output, then invokes the existing destruction wrapper and `remove`.
Linked player bodies use the linked user's existing `remove()` routine after
the command message. That routine tears down the input stack, mailbox/mailer,
body, owned call-outs, shell and connection. Ordinary objects retain the generic
destruction wrapper. Cleanup errors propagate through the existing shell handler.
Callback return values
are ignored; callback movement does not change the selected target. The existing
wrapper handles contained objects and normal equipment cleanup. A real stock
weapon is verified after crafting dependencies are initialized separately; its
cold first creation exceeded the existing 4000ms task limit.

Messages bypass command buffers, so overwritten files and downstream pipe output
are empty; append preserves existing text. Incoming file/pipe text is ignored.
Action, short and message errors stop cleanup; a `remove` error retains both
object and previously sent message. A self-removing `do_remove` leaves its object
gone and the later filename-status concatenation raises an error. Existing shell
handling catches these errors and rearms input; no guard or policy change is added.

`clean [object]` adds protected command dispatch and uses
`sizeof(all_inventory(...))` for the inventory emptiness check. With no argument
it cleans the caller's room; an explicit target stays while ordinary contents
are removed recursively. Living objects and real `add_item` decorations are skipped
by the command, and `do_remove` is not called. Removal callbacks run before the
ordinary clean message. Errors retain prior completed changes and return through
the existing shell handler; files/pipes remain empty while player messages go
directly to users.

The existing contents wrapper may move a skipped living child out of a removed
parent, then refuse recursive destruction of a protected attached decoration after
its ordinary move fails. In that case the parent and decoration remain, earlier
movement remains committed and no clean message is sent. The command retains this
shared cleanup/permission behavior.

`discuss object` adds protected dispatch and uses the account object's `is_user()`
role in place of `userp`, retaining the connected-body exclusion. This deliberately
protects never-connected Lima accounts and permits interactive ordinary objects.
Nine annotation-menu callbacks use bare function names; captured view/remove
indices retain their existing closures. Add/view/remove, editor cancellation,
yes/no retry, readonly author/admin rules and foreign readonly refusal are retained.
The daemon still keys annotations by base name and returns deep copies; saved
annotation classes restore through its ordinary startup method.

Menus, editor and pager remain interactive and bypass command output buffers.
Files/pipes are empty, append preserves prior text and incoming command text does
not supply editor input. Editor and pager completion return to the annotation
menu; quitting restores the shell. Existing pager blank lines and the menu's
source warning about concurrent annotation index changes remain.

`fix body` adds protected dispatch, keeps the parsed target in an `object`
local, clears its busy body reference with `nil`, and binds the next callback's
arguments with `call_out(&fix_body(body, ++step), FIX_PAUSE)`. The two-second
interval and existing class-identity check remain. Real body methods diagnose
styles, rebuild missing/invalid torsos and reroll zero stats. Call-outs preserve
the caller's player/body context under the port configuration; they do not have
an interactive call context. Diagnostics and target notifications bypass command
buffers, so files/pipes remain empty and incoming text is ignored.

An unexpected callback error stops the chain and leaves the command busy while
its remembered caller body remains live. Clearing the underlying fault alone
therefore does not restart it. This existing command limitation is retained;
normal terminal completion clears the busy reference and leaves shell input ready.

`start [object]` adds protected dispatch and retains the optional-object rule
and private handler. Querying reports the existing start location or configured
fallback; setting records the resolved object's filename without moving the body.
Living objects are refused. Nonliving objects, including live clones, remain
accepted; an unavailable stored path falls back without clearing the saved string.
The command does not save immediately. A later ordinary body save persists the
location, and the actual body constructor restores it. Ordinary command buffers
support overwrite, append and pipelines; redirected incoming text is ignored.

The unchanged body methods retain loadability checks and movement fallback after
a stored destination refuses arrival. START and WIZARD_START currently name the
same configured room, so their role-dependent branches do not yield different
paths under this port configuration.

`timer` adds protected dispatch, normalizes the repeat flag to an integer
boolean and supplies zero for an omitted notification interval. The daemon binds
the owner into callbacks and keeps one native call-out handle for replacement
and rescheduling. One-shot expiration,
periodic notices, repeat cycles, public/internal channel names and explicit owners
use the real timer/channel methods. Confirmations use command buffers, while later
notices go directly to the owner or channel; incoming text is ignored.

The daemon retains its existing global replacement behavior: setting another
owner's timer cancels the currently pending callback, although both owners' records
remain in query_timers(). Expired records also remain. `timer 0` returns
"Bad parameters." and does not stop a pending timer, despite the upstream command
comment. These existing source limitations are documented and retained.

`ilocate` adds protected dispatch to its existing private handler. The registered
single-string argument passes through the real IMUD locate module, which lowercases
the requested name. Confirmation uses command buffers, including files and pipes;
incoming text is ignored. Missing/excess arguments are rejected by command rules.
The rule also accepts quoted empty/spaced strings and flag-looking names; the port
retains that source behavior. Local checks capture the outgoing request at an
in-memory transport boundary; they do not confirm remote delivery or replies.

`mudinfo` adds protected dispatch to its existing private handler. It retains
raw arguments without a command-rule prototype, the local-name default, ASCII
case-insensitive glob-prefix matching, sorted cached records and status calculations.
Matching reports use command buffers for files/pipes, incoming text is ignored,
and the real pager returns to the wizard shell. Cached rows remain unchanged.

The no-match diagnostic still goes directly to the user through printf and bypasses
file/pipe routing. Quotes remain literal in raw patterns. An empty other-data mapping
prints an empty "Other data:" line; protocol zero omits that line. These source
behaviors are retained. Validation uses local cached metadata and does not contact
an intermud service.

`cmd` adds protected dispatch to its existing private handler. Its object-plus-
string-list rule resolves targets and joins words, preserving player-body refusal
and permissive handling of other objects. The action module forwards the raw
player-command argument, an empty flags mapping and nil input/output routes to
CMD. Empty-string route placeholders previously prevented the real version command
from running. Player-command lookup, parser fallback and NPC/player context
restoration remain unchanged; real NPC speech, emotes and version output pass.

Incoming file/pipe text is ignored. The wizard command does not capture target
output: direct target messages and player-refusal diagnostics bypass its output
routes. Objects without do_game_command still silently do nothing. The NPC action
module retains its raw player-command lookup without applying typed command rules
or adding pipelines. These source policies are retained; no native API is added.

`lima` adds protected dispatch to its raw version/pattern handler. Its daemon
restores saved history through explicitly bound callbacks, starts with an empty
mapping when no save exists, and uses native array regex matching with an explicit
match count. Typed absent results and the unused continuation use `nil`; the
numeric-zero version sentinel remains excluded from listing.

Sorted versions, help, raw argument parsing, chronological entries, case-sensitive
glob/anchor matching and multiline text retain their source behavior. Empty
versions report "No recent changes."; a pattern matching no entries retains the
report header. Command buffers support files and pipes, incoming text is ignored,
and the real pager returns to the wizard shell. Reads preserve the restored cache.

The command reports regex and oversized-report errors through its output buffer.
The driver's 8,192 UTF-8 byte string-concatenation limit remains; the shipped
`1.1a2` full history exceeds it and reports "overflow in string concatenation",
while narrower queries remain usable. The daemon's separate changelog export
method is outside these checks. No native API or shared policy is changed.

`objcount [minimum]` uses its existing protected dispatcher and native object
enumeration. Omitted and zero minima become one; it prints counts strictly above
the effective minimum, including any loaded prototype. With a prototype present,
`objcount 2` therefore prints a group containing two clones as a count of three.
When a prototype has been removed, the same total-count test remains; it does not
calculate clone counts separately. Clone retirement and prototype reload update
the next report. Negative minima include groups containing only a prototype.

The existing numeric command rule accepts signed integers, quoted numbers and
integer prefixes; `2junk` and `2.5` are parsed as two. Nonnumeric or quoted-empty arguments and excess
arguments retain usage diagnostics. Reports support files, append and pipelines,
ignore incoming text, and return from the real pager to the wizard shell. Six
focused checks cover thirty registered inputs and twelve complete compile checks.
No new command or driver code is needed; these counting and parsing policies remain.

`objfind <filename>` uses its existing dispatcher, native object enumeration and
mapping callbacks. It matches `base_name` exactly, including prototypes and
surviving clones, and reports each object's current environment or `nil` when
unplaced. Moving or retiring an instance and removing/reloading a prototype affect
the next report. The command replaces its stored result mapping on each query.

There is no typed command rule: missing arguments, literal quotes, relative names,
`.c` suffixes, clone suffixes, globs, flag-looking text and extra words simply
produce an empty mapping unless the raw text exactly matches a base name. Unicode
filenames work. Reports support files, append and pipelines, ignore incoming text,
and return from the real pager to the wizard shell. Five focused checks cover
thirty registered inputs and twelve complete compile checks. No new command or
driver code is needed; these filename-selection policies remain.

`idfind <id>` uses its existing dispatcher and asks each loaded object's `id()`
method whether it matches the raw text. Objects without that method are omitted;
prototypes and surviving clones are included when they identify themselves.
Lima's normal ID method matches aliases exactly and is case-sensitive. Multiword,
Unicode and literal quoted IDs work according to those methods; the command adds
no quote stripping or plural expansion. Environments update after movement,
retirement and prototype reconfiguration, with `nil` for unplaced objects.

Matched reports support files, append and pipelines, ignore incoming text and
return from the real pager to the wizard shell. Missing input prints "Idfind
what?" directly and keeps the previous stored report: file/pipe outputs remain
empty and append preserves prior bytes. A throwing `id()` method aborts the query,
retains the old results with the newly selected ID, and leaves later queries
usable. Seven checks cover thirty-nine inputs and fifteen complete compile checks,
including a real ID host. No new command or driver code is needed; these source
policies remain.

`profile` currently reports "Function profiling not enabled." through command
buffers. The port does not define `__PROFILE_FUNCTIONS__`; its enabled profiling
functions are excluded from the compiled command. No per-function timings or
profiling API are provided by this verified branch. Bare, filename, `-total` and
`all` forms retain the same diagnostic and ignore incoming text. Files, append
and pipelines preserve its exact bytes, and following shell commands remain
usable. Three focused checks cover twelve registered inputs and nine complete
compile checks. No new command or driver code is needed for this configured
branch; enabled function profiling remains separate work.

`parse` is reserved for parser debugging and keeps "Parse what?" for missing
input. Supplied text currently reports "Parser debugging is not available on
this driver." through command buffers. Native `parse_sentence` accepts but
ignores its legacy debug argument, so forwarding levels one/two would execute
matching actions and display the body's numeric result. The port command avoids
that body call until a real parser-trace API is available.

The unavailable diagnostic supports files, append and pipelines, ignores incoming
text and leaves shell state intact. Six checks cover eighteen registered inputs,
twelve complete positive compile checks and six direct native/body controls.
The original missing-dispatch and dispatcher-only controls are retained; the
adapted diagnostic performs no verb actions. Native parsing and the body module
are unchanged. Enabled parser debugging remains separate work.

`privs [path]` adds protected dispatch to its existing handler. It resolves raw
path text through the current wizard directory and reports configured read/write
privilege metadata. Existing and inherited entries, current-directory defaults
and relative/dot paths work. Quotes, flags, globs and extra words retain their
literal path meaning; no command-rule prototype is added.

The handler prints directly and bypasses command output buffers: overwrite/pipe
destinations stay empty, append preserves prior text and incoming text is ignored.
For `/`, it prints the first returned child entry rather than the root's effective
privileges. Under the initial configuration this yields `Read: -1` and
`Write: Mudlib:help`, although effective root values are zero/one. This existing
tree-entry selection policy is retained. Six checks cover twenty-nine registered
inputs and twelve positive complete compile checks. No daemon or native API
change is needed; reports preserve the tested configured trees and shell state.

`halt` adds protected dispatch to its existing zero-argument handler. It stops
combat for direct room occupants, clearing primary/secondary targets and cancelling
their repeating combat timers. Ordinary room objects without `stop_fight` are
harmless. Independent fights in another room or nested inventory keep their state;
the existing combat methods still remove reciprocal target references when needed.
Repeated calls, restarted fights, unused raw arguments and no active combat work.

The actor sees "You halt combat in the room." and nearby observers see the actor
halt combat. Carried bodies hear the room announcement; remote bodies do not.
These body messages bypass command output buffers: overwrite/pipe destinations
stay empty, append retains prior text and incoming text is ignored. Five focused
checks cover seventeen shell inputs and fourteen positive complete compile checks,
including actual target/timer state, message recipients and following shell input.
No native API, combat-module or message-module change is needed. Timed pulse
delivery and whole-library/startup qualification retain their earlier scope.

`resurrect [player]` adds protected dispatch to the existing handler. Bare input,
`me` and `myself` select your body; named lookup uses connected players, including
players in another room. Reincarnation clears the dead flag and restores disabled
limbs to one health point. It does not fully heal the body. Living self and missing
targets keep their diagnostics; living named players receive the existing action.

Action messages use ordinary names for dead bodies. Naming yourself follows the
named-target branch and delivers your message twice; its existing reflexive and
doesn't-need-help wording is retained. Case, quotes, flags and extra words remain
literal raw names. Direct write diagnostics and body messages bypass command output
buffers: overwrite/pipe destinations stay empty, append retains prior text and
incoming text is ignored. Six focused checks cover thirty-two shell inputs and
fourteen positive complete compile checks, using actual connected users, bodies,
lookup, reincarnation and messages. No native API or health/message change is
needed. Full death/corpse lifecycle and startup qualification retain earlier scope.

`killmobs <id|filename>` adds protected dispatch to its existing handler. Filename
selection strips optional `.c`, resolves absolute/relative/dot paths and removes
matching non-body clones across the whole object space, including other rooms
and carried inventory. Prototypes and bodies stay; ordinary nonliving clones
also qualify. Actual removal runs hooks and clears inventory membership rather
than performing combat death or awarding experience.

Name lookup retains existing path normalization: `flea` becomes `/flea` and does
not match a plain NPC name. An object named `/auditmob` matches normalized
`auditmob`; one named `auditmob` does not. Quotes, flags, extra words and globs
remain literal. Direct usage/count messages bypass command buffers: overwrite/
pipe destinations stay empty, append retains prior text and incoming text is
ignored. Six focused checks cover thirty-one shell inputs and sixteen positive
complete compile checks, with actual NPC/object removal and following shell input.
No native API, selection-policy or cleanup change is needed. The existing test
dummy's incompatible inherited method arity is recorded separately; the audit
uses Lima's flea NPC. Startup and full inventory qualification retain earlier scope.

`scripts` adds protected dispatch and inspects the actual NPC-script module.
The module adapts call-outs to bare function callbacks, absent references/status
to nil, state-update results to explicit integers and debug objects to explicit
formatting. Its API and step logic remain. Both LPC files retain CRLF endings.

Empty/prototype-only activity keeps the no-running message. Active clones in
other rooms and without an environment appear; inactive clones and prototypes
are excluded. Exact columns, truncation, current-step/count text, simplified and
ASCII progress bars pass. Real wait/description/action progression, callback
execution, debug output, completion and recovery are checked while inspection
leaves state and scheduled timer IDs unchanged. Direct printf bypasses command
buffers: overwrite/pipe files stay empty, append retains prior text and incoming
text is ignored. Raw arguments are ignored.

Six focused checks cover twenty-seven shell inputs and seventeen positive complete
compile checks, including Robert and the state daemon. No native API change is
needed. Explicit callback advancement is tested; timed delivery, full file-script
parsing, triggers and NPC-action routing await their own audit. Startup and full
inventory qualification retain earlier scope.

NPC script files now bind a function action's parsed method name separately from
its argument list. Consecutive integer/string argument lists and zero-argument
calls execute correctly; the former closure silently skipped the call while
advancing the script. The change preserves CRLF and the existing parser/step API.
Comments, blank lines, trailing newlines, absolute/relative paths, WAIT/DESC,
ordered `@@` commands and NPC-action routing are checked. Existing NPC targets
receive the action; missing targets and bodies are skipped.

Normal WAIT-to-TRIGGER handoff, real message matching, queued response execution,
pattern removal and following function completion work. Two existing limitations
remain: a first-step trigger is unarmed, and adjacent triggers prearm the later
pattern before the earlier one matches. An out-of-order later match can advance
prematurely. Six focused checks include sixteen positive complete compile checks
and five following shell inputs. Callback execution is explicit; broker timing,
arbitrary expression syntax and trigger-engine changes require their own review.

`smartmobs` adds protected CMD dispatch; its report logic and formatting stay
unchanged. The empty report prints the Object/Parses/Env/Mood header and ignores
raw arguments. Direct printf bypasses command output buffers: overwrite/pipe
destinations stay empty, append retains existing text, and incoming text is ignored.
The audit's four focused checks cover fourteen shell inputs and thirteen positive complete
compile checks, with two expected original compilation failures.

The behavior-tree compatibility changes allow actual Goblin clones to initialize
and appear in `smartmobs`, including clones in other rooms or without an environment.
Prototype exclusion, parse counts, moods, environment truncation and unchanged
inspection state are checked. Populated reports retain the direct-output routing
and ignored-argument behavior described above.

Five LPC files adapt reference defaults, class absence, typed callbacks, default
format_list separators, surplus callback arguments, navigation's assignment/return,
and the bandage's inherit alias/value conversion. Pending evaluation checks use
owned call-out introspection. Existing tree, leaf and targeting rules remain.
Seven focused checks include seventeen positive complete compile checks, two
expected original failures and sixteen registered shell inputs. Initial root and
leaf progression and explicit execution of an actual evaluation callback are
covered; full autonomous AI and timer lifecycle remain separate.

The fixture preloads all 21 stock recipes and the real crafting daemon before
creating Goblins. Cold setup reached the unchanged 4,000 ms budget while compiling
the crafting registry; these prepared-runtime checks do not establish cold startup.

The behavior-tree command runner passes literal arguments, empty mapping flags
and nil route slots to ordinary CMD handlers. The former empty-string tuple
skipped those handlers during route validation. Raw no-redirection commands and
native parser fallback retain their existing paths. NPC user/body context is
preserved inside handlers, and the original player is restored before returning
or rethrowing a handler error.

Movement, arrival and departure hooks use bare named function pointers, so moving
into a room registers the real arrival callbacks and records newly arriving beings.

Six focused checks include sixteen positive complete compile checks and five
following shell inputs. Actual evaluation callbacks execute look-around,
limb-configured own-status and study-beings, including real arrival hooks,
association registration and a parser-dispatched look at the newcomer. The
own-status check clears stale limb blackboard entries for the fixture's unwounded
state. Full leaf coverage, autonomous timing and cold crafting startup retain
their separate scope; the same 21-recipe warmup and 4,000 ms budget remain.

Full inventory, wizard/player-command and startup results retain their preceding
scope.

Current compatibility and handoff notes are in the neighboring lpc-rs checkout:
`doc/driver-comparison.md` and `local/lima/compatibility.md`. Temporary test
harnesses and execution logs belong in `/tmp/`. Continue source changes in this
checkout rather than making another source copy under `local/`.
