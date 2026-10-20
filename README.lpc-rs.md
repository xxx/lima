# Lima for lpc-rs

This is the working port of Lima based on upstream
`eefaff66adcae9b01e16059574c925b0d63e70a1`, with mudlib sources in `lib/`.
Use this dedicated checkout on `lpc-rs-port` alongside the native `lpc-rs`
checkout on `lima-support`.

**The port remains in progress.** The configured fresh-account lifecycle was
qualified on **19 October 2026** at Lima **8a26d8c7** and native **ded2bc3a**:
30 lifecycle checkpoints in 136.54 seconds, including character creation, ordinary
commands, native saves and saved login after restart. Both boots have 151 clean
preloads and exit normally. The five shipped suites pass 26/16/7/5/5, then both
combined modes pass 59/59. Source inventory and runtime results retain distinct scope.

| Qualification | Result | Scope |
|---|---|---|
| Current port acceptance | 30 checks across two clean driver processes | First administrator; actual saved-password authentication and character identity/state verified after restart. |
| Ordinary-player lifecycle | 57 checks across two clean driver processes | AUTO_WIZ disabled in the disposable runtime; real player shell, commands, saved-password login, quit location and alias restoration. |
| NPC adapters | Both complete hosts compile; 48 focused runtime checks pass | Aggression timers and trainer skill conversations; default stat-menu qualification follows. |
| Trainer stat menu | Four source targets compile; 43 runtime checks pass | Adopted source with default strength/agility options; prices, payments, point exhaustion and cleanup verified. |
| Shipped tests | Five suites pass 26/16/7/5/5, then both combined modes pass 59/59 | Current release driver; per-suite commands first, no manual program loads. Per-suite-first is the operator recipe; the current direct-cold recheck is below. |
| Whole-source inventory | 1010/1182 compile; 172 standalone failures | Fresh sweep after the collection helpers; no regressions/timeouts/crashes, target constructors omitted. |
| Stone/sword sample | Eight scoped targets compile; two-player behavior passes | On-demand sample, including refusal, extraction/return and cleanup. |

Of the inventory's failures, 113 documented fragments have currently compiling
shipped hosts. **The other 59 consumers remain open for porting or explicit
review.** Optional or unselected code is not finished merely because it is absent
from the configured boot. These are not counts of missing driver features.
Command compilation remains player 65/67 (random2 deferred; suicide disabled
upstream), wizard 81/81, guild 4/4 and verbs 90/90.

Fixed parameter defaults follow the native
[trailing-default rule](../lpc-rs/doc/lpc/anatomy_of_a_program.md#parameter-defaults).

## Build, check and start

From the sibling native checkout, build the workspace:

```sh
cd ../lpc-rs
CARGO_BUILD_JOBS=2 CARGO_INCREMENTAL=0 cargo build --workspace --locked --release
cd ../lima-lpc-rs
```

Use the profile from this checkout's root: `LPC_LIB_DIR=lib` is relative to
the working directory. It selects distinct nil, preserved call-out player
context and a 4000ms execution limit. `lpc-config.json` is editor metadata.
Exported environment values override values in `lpc-rs.env`.

Compile a complete command without running its constructor:

```sh
../lpc-rs/target/release/lpc-rs-lpcc -c lpc-rs.env --check lib/cmds/player/mudlist.c
```

Startup uses `-e` for the profile. For the administrator test commands below,
prepare a wizard directory matching the account you will create; this example
uses `portadmin`. On fresh data, the first wizard becomes administrator when
no administrator exists. Subsequent AUTO_WIZ accounts are guest wizards; that
alone does not grant the evaluation command's file-write permissions.

The checkout enables `AUTO_WIZ` for development. For ordinary new players,
remove that definition in `lib/include/config.h` before starting the instance.
The existing no-administrator bootstrap still makes the first account an
administrator; later accounts use the Player Shell. A separate disposable-runtime
audit qualifies this setting with the same distinct-nil/preserved-player profile.

The committed preload includes IMUD. Its router is configured in
`lib/daemons/imud_d.c`, and its OOB listener uses the game port plus one.
Choose those service settings before starting. Runtime qualifications used
disposable copies with a loopback router and an explicit loopback OOB bind;
the game bind below alone does not select those settings. HTTP, FTP and RCP
remain commented out in `lib/data/config/preload`.

```sh
mkdir -p lib/wiz/portadmin
LPC_BIND_ADDRESS=127.0.0.1 LPC_PORT=4000 \
  ../lpc-rs/target/release/lpc-rs-driver -e lpc-rs.env
```

Connect a Telnet-capable MUD client to `127.0.0.1:4000`. Follow the account and
character menus, create/select a human character, and enter the game. Try
`look`, `inventory` and `pwd`. `quit` returns to the account menu, where `q`
disconnects; reconnect to check saved account/body restoration. The administrator
command `shutdown <reason>` stops the driver normally.

Startup writes saves, generated help and logs below `lib/`. Keep those runtime
files with the instance; use a disposable `/tmp/` copy for throwaway checks
rather than replacing or repeatedly copying the persistent port source.

## Run the shipped tests

Use the administrator wizard shell. Send each command separately and wait for
the shell prompt:

```text
unittest list
unittest items
unittest limbs
unittest liquids
unittest race
unittest salvage
unittest all
unittest all quiet
look
pwd
```

The five suites should report 26, 16, 7, 5 and 5 successes respectively. Both
combined commands should then report 59 successes and zero failures. Normal
mode prints case results; quiet mode prints aggregate totals. This sequence
passed from a fresh driver after normal startup under the unchanged 4000ms
limit, with no manual program loads or added LPC fixtures. The current release
qualification also checks existing prototype identities, player/inventory state
and cleanup, then saves, logs out and verifies the account and character after
a fresh-process restart. Manual totals alone do not repeat those observations.

The [current direct-cold recheck](../lpc-rs/doc/driver-comparison.md#lima-current-cold-combined-checks)
passes both modes from fresh drivers without per-suite warmup or manual loads.
Normal-first returns the prompt in 2.330s and quiet-first in 2.542s; each following
other mode also passes 59/59, at 0.185s/0.189s. Both boots have 151 clean preloads,
preserved player/world/prototype state, normal saves/logout and exit-zero shutdown.
The 4000ms budget and source/profile/binaries are unchanged.
Evidence: /tmp/lima-cold-current-20261019.

The earlier timeouts predate the lexer/include optimizations. Those attempts spend
about 3847/3867ms compiling and 155/152ms evaluating, with zero conflicts. Their
command/prompt output rolls back; ordinary look recovers input and state is
preserved. They remain recorded failures, while the current recheck does not
reproduce them. Two passing trials do not guarantee reliability under load;
response timings include scheduling/network, exclude the separate drain and do
not measure compiler phases. OS caches are not flushed. Keep the per-suite-first
recipe; this recheck alone does not justify another compiler change.

Two shell observations inspect initially loaded objects before testing; they
compile evaluation code without loading test targets. The per-suite sequence
separates compilation across tasks and remains the qualified recipe. These
cold observations identify compilation pressure. A later compiler-stage profile
attributes 61.5–62.8% of measured compilation to preprocessing and 16.0–16.5% to
code generation; its ordinary disabled/enabled controls pass both modes at
59/59. The restored uninstrumented driver also passes a fresh cold run. These
passes do not establish cold reliability or a speedup. The deadline, source
and cache policies are unchanged. Details are in the native
[compiler profile](../lpc-rs/doc/driver-comparison.md#lima-cold-compilation-profile).
The [cached-macro copy audit](../lpc-rs/doc/driver-comparison.md#lima-cached-macro-copy-audit)
keeps the current storage: its isolated replay does not establish a worthwhile
whole-compiler gain. A subsequent
[lexer token-text audit](../lpc-rs/doc/driver-comparison.md#lima-lexer-token-text-copy-audit)
finds 8.7–10.6% lower raw-lexer time after removing a redundant text buffer, with
3,984 matching token/error/span comparisons. The small native
[buffer removal](../lpc-rs/doc/driver-comparison.md#lima-lexer-buffer-removal)
is now applied. Eight fresh driver sessions pass the combined or per-suite test
sequences with state/cleanup checks, real saves and logout. Actual compiler and
cold-command timings remain noisy; no whole-compiler gain or cold-timeout fix is
established. The mudlib and profile are unchanged. The locked release workspace
build, 5,161 workspace tests/doctests (two ignored), warning-free all-targets
Clippy and format check pass. The ordinary release workspace rebuilds after
testing, and its driver/CLI hashes match the measured executables exactly.

The subsequent
[include resolution audit](../lpc-rs/doc/driver-comparison.md#lima-include-path-resolution-audit)
finds that at least 94% of observed metadata checks cannot change the selected
header path. A small resolver short-circuit preserves the tested search and
callback behavior and substantially reduces isolated resolution time. It is
now adopted in the native
[resolver](../lpc-rs/doc/driver-comparison.md#lima-include-resolution-short-circuit):
actual preprocessing metadata calls fall from 13,088 to 785, with unchanged
header reads and callback order. Eight fresh driver sessions pass; sampled cold
command and CLI medians improve 2.5% and 14.6%, respectively. The samples do not
establish cold-timeout reliability. The locked release workspace build, 5,167
workspace tests/doctests (two ignored), warning-free all-targets Clippy and format
check pass. After rebuilding the ordinary release workspace, the final driver
also passes two fresh sessions with both combined modes at 59/59, clean preloads,
preserved state, native saves and logout. The mudlib and profile are unchanged.

The earlier 42-program manual-load recipe belongs to the historical prepared
run. The current sequence uses the existing test commands instead. CLI compilation
does not warm a running driver's object space.

## Ordinary-player lifecycle

With stock `AUTO_WIZ` disabled only in a disposable runtime copy, **57 checks
across two fresh driver processes pass in 9.26s**. Both boots load 151 preloads
cleanly and exit zero. The ordinary account stays non-wizard/non-admin and uses
`/obj/mudlib/pshell` (`Player Shell`). Real menus, human creation, look/inventory/
score, aliases, movement and normal menu logout pass.

Native account, character and player-shell saves survive restart. Real password
login restores identity, role, race, capacity, empty inventory, command path,
alias and the saved Example Room quit location. Existing channel compiler
warnings remain; there are no runtime errors or failed preloads. This requires
no port source fix and does not change this checkout's development setting.
The [native qualification](../lpc-rs/doc/driver-comparison.md#lima-ordinary-player-lifecycle)
records scope; local evidence is /tmp/lima-mortal-entry-20261019. It does not
repeat the shipped suites, full gameplay coverage, foreign saves or live services.

The [current acceptance record](../lpc-rs/doc/driver-comparison.md#lima-port-acceptance)
reuses the documented per-suite-first recipe on this checkout, with preserved
player/world state, zero manual target loads, completed documentation scanning
and real saved-password authentication. Sources, profile and release binaries
are unchanged. Existing channel/header warnings remain; no runtime errors or
preload failures occur. Evidence: /tmp/lima-port-acceptance-20261019.

## CGI helper regression tests

The three `WWW/cgi` helpers now compile and pass 55 isolated runtime assertions.
`autodoc.c` uses ordinary HTML string literals, distinct nil routing and bare
callbacks for its indexes and inheritance tree. `mudinfo.c` uses native array,
mapping-filter and regex-result forms. `who.c` supplies the four required colour
wrapper arguments and handles omitted query/form names. Empty queries are
handled before indexing.

Run the permanent regression test from this checkout:

```sh
python3 tests/cgi/test_cgi.py
```

The [test guide](tests/cgi/README.md) explains compiler selection and fixture
scope. The runner copies current sources into a temporary library, uses the
existing profile, and exercises generated documentation, mud filtering, colour
stripping, finger fields and the HTTP form's data-plus-headers call. The real
class definitions, grammar, glob and ANSI providers remain in use; IMUD and
finger records are controlled fixtures. This qualifies helper output without
starting an HTTP service or contacting a router.

The full sweep immediately after the CGI adaptations measured **1008/1182 compile, 174 fail**, with
zero regressions/timeouts/crashes. All three gains are these CGI helpers; 113
hosted fragments and 61 other consumers remain. This is compilation coverage,
not another full lifecycle or live-service qualification.

## Queue and stack regression tests

The contributed collection helpers now compile and pass 37 isolated LPC runtime
assertions. `last_queue.c` returns a `mixed` element from `lqueue_member` and
returns one fresh capacity-sized window at index zero, including after wrapping.
`stack.c` uses native dot access for the class field in `stack_pop`.

```sh
python3 tests/collections/test_collections.py
```

The [test guide](tests/collections/README.md) describes the fixed-window, alias,
mixed-value and overflow/underflow cases. The fixture inherits the two actual
helper programs. It uses the existing profile in a temporary library without
starting a service. The accessor declaration is already `void` in the pinned
upstream source; a return-type-only control exposes the doubled zero-index window
under native slicing. No upstream driver was executed.

The following full sweep measures **1010/1182 compile, 172 fail**, with zero
regressions/timeouts/crashes. The two gains are these helpers; 113 hosted fragments
and 59 other consumers remain. The port is still in progress.

## Tar writer audit

`contrib/tar.c` remains unported. Its mutable header can be represented by an
integer array and converted with `to_bytes`; no mutable-byte API is needed.
A small native control archive preserves binary bytes and an empty file when
read by Python and GNU tar, but that control does not execute this utility.

A driver issue must be fixed first: `write_bytes` checks physical metadata,
ignoring the calling transaction's pending creation, growth and truncation.
The audit reproduces failed writes after creation/append and incorrect negative
offsets or bounds after staged changes. Keep the usual missing-file zero result,
but use the current transaction's file view for existing pending files.

The tar source also needs native callback binding, explicit archive creation,
empty-file/directory handling, terminating zero records, corrected large-file
record arithmetic and bounded continuations. Its filename-field limits remain
part of the format decision. See the [native audit](../lpc-rs/doc/driver-comparison.md#lima-contributed-tar-audit).
The source inventory remains 1010/1182 compiling with 59 open consumers; no tar
adapter is adopted by this audit.

## Remaining scope

Original-FluffOS saved-data/password import, alternate build options, live I3
and other optional service operation require their own decisions and checks.
Some optional/unselected sources and documented original Lima defects remain;
the inventory is not a port-completion percentage. The inherited-method
signature/cache design discussion also remains separate.

The 16-entry optional-source review has six adapted entries and one unchanged
boss host qualification. The other nine retain these decisions:

| Source | Current decision |
|---|---|
| `m_exit_obj.c`, `std/room/exits.c` | Keep the selected simple/complex exit providers; restore these older APIs only for a concrete consumer. |
| `state_of_mind.c` | Keep the selected condition module and its two-argument stun API. |
| Duplicate `domains/std/object/hole.c`, WIP `std/paper.c` | Keep selected `std/hole` and `domains/std/paper`. |
| `secure/error_handler.c` | Keep the configured master's active error handler. |
| `portable_board.c` | Select a compatible board provider before adaptation. |
| `clan_d.c`, `secure/modules/m_file.c` | Incomplete subsystems/contracts remain deferred. |

These are recorded scope decisions, not a queue of missing driver features.

The [NPC sample-chain audit](../lpc-rs/doc/driver-comparison.md#lima-npc-sample-chain-audit)
now has [aggression/trainer adapters](../lpc-rs/doc/driver-comparison.md#lima-aggression-and-trainer-adapters)
and focused runtime qualification. Aggression binds targets into function values;
the trainer uses native class fields and a mapping restriction contract. Both
complete hosts compile. All 48 earlier runtime checks pass for inherited timers
and skill conversations, with state restoration, cleanup and normal saves/logout.
The timer fixture records attempts; combat damage is outside that check.

Tara's default stat-menu gap is now resolved by the four-line
[trainer adaptation](../lpc-rs/doc/driver-comparison.md#lima-trainer-stat-menu-adoption).
It uses the body's existing `stat_abrev()`, `query_stat()` and `inc_mod_stat()`,
and binds the selected stat in its menu callback. Four source targets compile;
the adopted checkout's source passes the same 43 runtime checks as the audit,
with default stat options enabled and no module substitution. Prices, refusals,
exact payments, repeated strength/agility gains, point exhaustion, reopened menus,
state restoration and normal saves/logout/shutdown pass with 151 clean preloads.
No new body/driver API, payment cast or sample configuration change is needed.
Evidence: /tmp/lima-trainer-stats-adoption-20261019.

The attic reaction examples retain their upstream placeholder; their missing
script API stays deferred. The dated 999/1182 inventory is not rerun. Alternate
stat models and configuration flags retain their separate scope.

The [remaining-source triage](../lpc-rs/doc/driver-comparison.md#lima-remaining-source-implementation-triage)
reviews the 16 files with no identified configured caller as one batch. Temporary
adapters compile three optional examples; other entries need hosts, integration,
dependencies or unfinished feature work. No source change or new efun is added.
The audit also reproduced a compiler input difference, corrected in the
[relative-inherit follow-up](../lpc-rs/doc/driver-comparison.md#lima-relative-inherit-inputs):
host-file and in-game inputs now use the source directory. All 12 CLI cases and
both reordered behavior-example forms pass. Evidence: /tmp/lima-relative-inherits-20261019.
The behavior example now places its inherit before its global declaration and
retains the relative sibling path. Both actual sources and a child consumer
compile; four constructor checks cover the inherited global and class fields.
The class provider and profile are unchanged. This is an optional example's
inheritance check, without gameplay or a new whole-library inventory. Contrib's
drunk effect retains its parent's heartbeat adaptation gap.
Evidence: /tmp/lima-behaviour-example-20261019.

The optional [navigation passage example](../lpc-rs/doc/driver-comparison.md#lima-navigation-passage-example)
now passes an explicit float mass to the existing API. Four compile checks and
12 checks with real clone construction pass, covering mass, IDs, attachment,
descriptions, independent state and cleanup. The isolated fixture uses the shipped
providers/simuls and a temporary master; configured gameplay remains unqualified.
The profile and native binaries are unchanged. Evidence: /tmp/lima-navigation-passage-20261019.

The optional [Logical Genius adapter](../lpc-rs/doc/driver-comparison.md#lima-logical-genius-and-trigger-defaults)
uses native typed closures and nil rule references. The trigger APIs now default
omitted rule references to nil while retaining the numeric listen mask and strict
checks on explicit zero. Eight complete-consumer/fixture compiles and 40 runtime
checks pass, including predicates, captures, message matching and real clone setup.
Response commands are captured in the fixture; delayed speech/player gameplay is
not newly qualified. The profile and native binaries are unchanged.
Evidence: /tmp/lima-logical-genius-20261019.

The optional [test dummy adapter](../lpc-rs/doc/driver-comparison.md#lima-test-dummy-example)
uses native event fields/callbacks, nil formatter separators and the inherited
give-checker signature. Six compile checks and 25 runtime checks pass with real
dummy/equipment clones, arrival hooks, damage diagnostics, armour reduction and
cleanup. The registered stand-up callback is invoked directly and cancelled;
elapsed timer delivery and player gameplay are not tested. The profile and native
binaries are unchanged. Evidence: /tmp/lima-test-dummy-20261019.

The optional [character-mode key-binding helper](../lpc-rs/doc/driver-comparison.md#lima-character-mode-key-bindings)
now iterates characters directly and registers a bare input callback. Four compile
checks and 31 runtime checks pass through a temporary wizard-shell composition
and the real input provider, with a recording user at the modal boundary. Matching,
Escape/control tokens, dynamic bindings and Unicode are covered; owned-shell
startup, the real user stack and network input are outside this check. No configured
caller is identified or enabled. Evidence: /tmp/lima-key-bindings-20261019.

The optional [flow room module](../lpc-rs/doc/driver-comparison.md#lima-flow-room-module)
now retains one named arrival callback. Five compile checks and 27 runtime checks
pass with temporary room compositions, real movement and message providers.
Repeated configuration, conditional flow, inventories and delayed captures/guards
are covered. Delayed callbacks are invoked directly and cancelled; elapsed timer
delivery is not tested. No configured caller is enabled; the profile and native
binaries are unchanged. Evidence: /tmp/lima-flow-room-20261019.

The unchanged [boss module](../lpc-rs/doc/driver-comparison.md#lima-boss-module-host-contract)
passes five compile checks and 17 runtime checks in temporary ADVERSARY hosts,
with fixture actors and a recording loot daemon. Host methods, eligibility/history
and handoff arguments work without a port fix. Its existing unused loot flag and
unrefreshed expiry timestamp remain documented policy gaps. Controlled dead-state
and timestamp fixtures do not qualify actual boss combat or generation; no caller
is enabled. Evidence: /tmp/lima-boss-integration-20261019.

The [legacy exit integration review](../lpc-rs/doc/driver-comparison.md#lima-legacy-exit-integration-decision)
retains BASE_ROOM/M_EXIT and EXIT_OBJ/M_COMPLEX_EXIT. Six compiler passes, four
expected rejections and ten runtime checks show that the unselected older helpers
need more than argument-count edits: destination lookup, callbacks and movement
records have different contracts. The fixture records movement arguments; it
does not move a body. No diagnostic adapter or library change is adopted.
Evidence: /tmp/lima-exit-integration-20261019.

The current native [driver comparison](../lpc-rs/doc/driver-comparison.md#lima-port-readiness)
centralizes driver differences and qualification scope. The records below retain
the detailed local evidence, including nonpassing controls; temporary `/tmp/`
paths are evidence locations on the development machine, not installed tooling.

## Historical qualification notes

These records describe their dated revisions. Earlier totals and statements of
pending work are superseded by the current summary above and later results.

### Release lifecycle qualification

Reviewed 18 October 2026 at native **cae3c8cd**, Lima **b5580f53**. Two fresh
release-driver processes pass all **30 lifecycle checks** in **136.6s**, with
**151 clean preloads per boot** and exit status zero for both processes. The five
suite commands pass **26/16/7/5/5**, then normal and quiet combined modes each pass
**59/59**.
Existing prototypes and player/world state are preserved; the observation
marker is removed. The first documentation scan completes before shutdown. Real
saved password authentication and character name/program/race/room/inventory/material
pouch restoration pass after restart.

No source changes, manual target loads or added LPC fixtures are required.
The distinct-nil, preserved-player, 4000ms profile and all source/binary hashes
remain unchanged. Existing channel compiler and autodoc header warnings remain;
there are no runtime errors or preload failures. The two existing loopback
IMUD substitutions and fixed local router frames retain their limited scope.
This first-administrator/native-save result does not qualify original-driver
save import, live services, cold combined reliability or mortal-only behavior.
Evidence: /tmp/lima-release-lifecycle-20261018.

### Release inventory after trailing-default validation

Reviewed 18 October 2026 at native **0e87ead9**, Lima **a1984c30**. The release
workspace is rebuilt, and invalid function/closure probes verify the new compiler
rule. The bounded source sweep measures **999/1182 compile, 183 fail, zero
regressions/timeouts/crashes in 45.643s**. All 998 previous successes
remain, with sword_in_stone as the one new success and no target-set changes.

All 111 reviewed fragment proofs retain passing complete hosts (45 distinct hosts);
the other 72 failed standalone files retain prior contexts. The resolved sword
child leaves 23 carried consumer decisions; all remaining first diagnostics match.
Core and command groups remain green within their recorded scope. Only xterm256_d
and sword_in_stone changed since the prior inventory; headers/profile match.

Two CLI workers, two Tokio workers each, 4GiB/40s per target and the normal distinct-
nil/preserved-player/4000ms profile remain. All 1549 library files and binaries
stay unchanged during the run. Target constructors do not run. The baseline used
target/debug and this sweep uses target/release; timing is not a performance
comparison. Earlier gameplay and suite results retain their separate scope.
Evidence: /tmp/lima-source-inventory-defaults-20261018.

### Colour-helper parameter defaults

Reviewed 18 October 2026 at Lima **c0ae3baf**, native **9f1fc65e**, with the
following source/notes edits pending. The earlier port gave substitute_ansi and
substitute_colour a default text before a non-default mode parameter. The
native default-position defect then replaced supplied text with an empty string
when mode was omitted. Both declarations now explicitly default mode to an
empty string, retaining their existing plain fallback and explicit-mode behavior.

Two real administrator sessions exercise 58 cases each: the original has six
lost-text cases, and the tested adapter fixes all six. Explicit plain/ANSI/VT100/
Xterm, empty/nil mode, no arguments and empty text retain their results. Six real
source targets compile. Both sessions preserve player/colour/daemon state and
finish normal saves/logout/shutdown with 151 clean preloads. Only those two
parameter defaults change; native code and the 4000ms profile are unchanged.
The compiler's non-trailing-default issue remains separate. The observer's first
unescaped-colour JSON run and policy-blocked CLI attempts remain failed controls.
Evidence: /tmp/lima-parameter-defaults-20261018; the source inventory is not rerun.

### Earlier qualification records

The standalone-source inventory, refreshed **17 October 2026** at native
**14d766dc** and this checkout **9cddebfc**, compiles **994 of 1,182 .c files**:
**188 fail, zero timeouts/crashes, 603.911s**. All **985** preceding successes
remain; **9 previous failures now compile**. No sources were added or removed.
Gains cover debris, guild guard/example, book/Lima guide, ladder, meat, pelt and
climb challenge. Eleven library sources changed since the preceding inventory;
configuration and compiler/driver binary hashes match.

The full inventory established **82 failed fragments** with passing complete hosts.
The focused classification below adds **28** source/host proofs, giving **110**
confirmed compositions against the same 188-file failure set and **78** remaining
review items. This is not a new whole-source count or a native-gap count. All 34
standard-room, 22 junk, 25 guild and 18 consumable compile successes retain their
measured scope; target constructors did not run in that sweep.

Maximum two CLI processes, two Tokio workers each, 4 GiB/40s per target and the
normal 4000ms/distinct-nil/preserved-player profile remain. All 1,549 library files,
configuration and compiler binary stay unchanged during the run. This is not a
performance benchmark, Rust gate or gameplay rerun. Prior 2367+10 full replay and
later focused runtime checks retain their scopes. Evidence:
/tmp/lima-source-inventory-refresh-20261017; prior /tmp/lima-source-inventory-20261017.

The digging audit and source adoption below record the nested prompt repair and
its production replay. HOLE resolves to `/std/hole`, not the failing duplicate in
`domains/std/object/`. The source inventory and room-construction results retain
their original scopes.

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

The mailbox/leaflet source still exactly matches the audited two mass conversions;
implementation and notes are pending commit. All **83 prior compile checks** are
retained for unchanged source/configuration/binaries. The new full retry passed
**all 62 mailbox inputs** and earlier helpers through normal missions, with one
clean 151-preload boot. Full validation remains **pending**: it ended normally in
**343.78s** during mission-expiry polling, before saved-state restart.

Input callback resolution exceeded 4000ms with **zero compilation time**. GC then
reported reclaiming **2,272,887 variables**, and a live-snapshot reply found its
receiver cancelled. Callback snapshot acquisition shares the evaluation deadline;
GC executes atomically in the committer. GC/snapshot waiting is suspected, not
isolated or fixed. This differs from the earlier cold-preload failures, preserved
under /tmp/lima-mailbox-props-20261016. Source/helpers/fixtures/limits are unchanged;
no native change or current full-replay success is claimed.

Same six fixtures/two address overrides, distinct nil/preserved player/4000ms and
actual master/simuls/user/body/parser; no new LPC helper/body/host/grant. Prior
2082 + 10 restored-favour / 426.29s replay and 966/1182 sweep keep measured scope.
Next investigate the native snapshot/GC deadline before another full qualification
or the next container-example audit. Evidence:
/tmp/lima-mailbox-props-retry-20261016.

Mailbox source/validation notes are committed as **4c15b53b** (native **8561a764**).
The native timeout audit now has two passing paused-clock tests: snapshot queue
waiting consumes the existing evaluation allowance, and cancellation releases the
snapshot pin. Native documentation explicitly explains this behavior. No production
GC, deadline, mode/configuration or mudlib change is made by that audit.

The recommendation is a measured, selective GC reconstruction path for garbage-
heavy sweeps while preserving the current timeout and atomic/quiescent collection
contract. Debug scalar-world and optimized map-only fixtures favor reconstruction
when nearly everything is garbage; unconditional reconstruction was slower for
mostly-live maps. These are not an end-to-end Lima speedup. Temporary profiling
code is excluded from the shared change. Full replay qualification remains pending;
all 62 mailbox inputs passed the preceding retry. Evidence:
/tmp/lpc-gc-snapshot-audit-20261016.

The subsequent native collector change selects reconstruction from traced cell
IDs and protected identities when their conservative survivor bound is at most
10% of the world. Other collections retain individual deletion. Snapshot metadata,
object lookup, atomic/quiescent collection, wall-clock accounting and the normal
GC period are preserved; no Lima source or configuration workaround is added.

The ordinary full replay with that change passes **2,159 first-session inputs +
10 restored-favour inputs**, **30 checkpoints**, two clean **151-preload boots**,
and all retained gameplay, including the exact **62-input mailbox sequence**, in
**474.85s**. Same six fixtures/two address overrides and distinct nil/preserved
player/4000ms remain. Normal periodic GC reconstructs a **2,186,372-cell** world,
reclaiming **2,157,205 cells** with a **2,775.341ms** collector pause; no transaction
timeout or cancelled snapshot reply is reported. Account/body/bank/favour state
restores. The separate original-collector measurement used a smaller world and
is not a matched native speed comparison. Earlier compile checks remain prior
83/83 evidence, not rerun for the GC-only change. Cold-compilation investigation
stays separate; next audit is the standard table/treasure-chest consumers. Evidence:
/tmp/lpc-gc-sweep-20261017.

The subsequent GC parallelism investigation leaves driver code, dependencies,
storage and Lima source/configuration unchanged. The same 596-command measurement
prefix retains 15,264 of roughly 899,000 cells. Debug construction/destruction/
collector times are **21/735/885ms**; the optimized native driver measures
**6/235/268ms**. Destruction dominates. These are measurement-only runs; the prior
complete 2,159+10 replay remains qualification.

Rayon iterator adapters lose in the separate optimized map fixture. An isolated
imbl API prototype exposes owned root branches without changing map layout and
measures **197ms serial -> 114ms with two Rayon workers** for 2.3m entries with
Arc payloads. This unpublished API is not integrated into the driver and the result
is not a native speedup. A bounded native prototype with dependency API support
and transaction/GC validation is the recommended next performance investigation.
All temporary timing code is removed and debug/release production binaries rebuilt;
atomic GC and the existing timeout/mode contracts remain. Evidence:
/tmp/lpc-gc-parallel-20261017.

The subsequent native worker prototype lives only in the detached checkout at
/tmp/lpc-gc-native-20261017/checkout (native d2a2c28b). Its unpublished imbl
owned-branch API and Rayon dependency do not change production code, storage or
Lima source/configuration. Two lazy workers finish retired-map disposal inside
atomic GC; small maps remain serial and worker creation failure falls back.

The prototype passes **5,134 workspace tests / 2 skips**, five new lifetime and
completion tests, **10 doctests**, warning-free Clippy/docs, formatting and the
optimized workspace build. One paired measurement uses the same binary and
596-command prefix, six fixtures/two address overrides and ordinary GC period,
with distinct nil/preserved player/4000ms unchanged. Both retain **15,264 cells**
from roughly 899k. Serial disposal/collector service measure **275.1/315.7ms**;
two-worker disposal of 32 owned branches measures **143.8/184.0ms**, a **41.7%**
shorter collector pause in this pair. These are prefix measurements; the prior
complete 2,159+10 replay remains full qualification.

All **362 transaction workload trials** pass. Initial large differences do not
persist in nine longer paired repetitions of affected cases; repeated medians
range from **8.2% faster to 2.5% slower**, with overlapping ranges. Zero overhead
is not established. Results support the approach, but adoption needs a maintained
dependency API strategy; the temporary path and measurement control remain isolated.
Native timeout/mode contracts, prior 83/83 compile evidence and 966/1182 inventory
keep their measured scope. Evidence: /tmp/lpc-gc-native-20261017.

The subsequent published-API research tests the unmodified registry imbl 7.0.2
with the native allocator version. All 700 measured container trials pass, covering
batched iteration, payload extraction, mutable pipelines, partially consumed map
copies, two/four/eight stored maps, snapshot/read/update costs and alternate pointer
backends. Two maps with parallel disposal reduce owned-payload collection medians
**68.8->37.5ms** at 900k entries and **276.9->152.5ms** at 2.3m. Serial two-map controls
are 57.1/239.3ms, so the gains include both layout and parallelism. Snapshot clone/drop
rises from roughly 13ns to 24–25ns; sampled read/update medians improve 1–14%, with
read ranges overlapping. These are container results, not native/Lima speedups.

Recommend a bounded two-map native prototype under the existing single snapshot
version/committer, preserving atomic/quiescent GC and deadline accounting. Workers
must join before GC returns. Iterator pipelines regress scalar cleanup substantially;
alternate pointers show no convincing owned-payload GC gain. The uncommitted vendor
proposal is withdrawn and preserved in its prior evidence package. Main native
code/Cargo settings are restored to the original registry dependency; workspace
build passes. No native storage change or new full Lima replay is claimed.
Evidence: /tmp/lpc-gc-public-api-20261017.

A further matched container comparison uses one executable, identical 32-byte
payloads/survivor IDs/hash seed/allocator and two workers. Seven balanced rotations
complete **196 measured trials**. Patched branches versus two public maps measure
**33.4/26.9ms** for owned payloads at 900k entries, and **124.0/125.7ms** at 2.3m,
where ranges overlap substantially. Scalar results are 13.6/8.1ms and 61.5/59.1ms.
Four maps reach 78.8ms in the larger owned case, with more handle-copy overhead.
The partition algorithm calls unchanged public methods; both cases link the local
API extension solely to control the dependency build. Main native source and
registry configuration remain unchanged; native validation is recorded below.
Evidence: /tmp/lpc-gc-matched-20261017.

The native follow-up supports **two maps without a fork/vendor**. With the same
596-command prefix and normal 300-second GC interval, optimized total pauses are
**263.9ms unchanged**, **169.4ms patched imbl**, **178.2ms two public maps**, and
**193.1ms four public maps**; every parallel variant uses two workers. All runs
pass, collecting about 899k cells to 15.3k with 656 roots. These are single GC
samples, not identical heaps: the control retains 15,268 cells versus 15,264 for
the other variants. The observed two-map pause is 32.5% shorter than the control.

Both partition variants pass 5,138 tests/2 skips, including nine new ownership,
routing and completion tests, plus the workspace quality checks and release build.
All 560 transaction trials and 96 longer repeats pass. Larger initial slowdowns
do not persist; two-map recheck medians range from 2.0% faster to 2.5% slower,
with overlapping ranges. Two maps remain a viable alternative under one snapshot,
version and committer, with synchronous worker completion. That prototype remains
isolated and original public executables were restored after the comparison. This does not add a full replay or inventory
qualification. Evidence: /tmp/lpc-gc-world-native-20261017.

The selected delivery uses the existing single world map and a direct Git pin to
[the imbl fork](https://github.com/xxx/imbl/commit/5fa322d043faee90d32c63430b51a52be3ad121d).
The API/tests commit is separate from the upstream release lockfile correction.
Seven ownership contract tests run in the normal native workspace gate, and the
fork records how to replay the feature onto reviewed upstream releases. No vendor
copy, root patch override or preparation tool is required by Git/path consumers.
Existing dependency versions and feature selection remain unchanged. That change
prepared the dependency API without enabling workers. Lima source, configuration
and qualification scope were unchanged. Evidence: /tmp/lpc-imbl-fork-20261017.

The native collector now disposes of large rebuilt maps using a lazy two-worker
pool through the pinned owned-branch API. Small/shared/direct cases and worker
creation failures clean up synchronously; workers finish inside the accepted
atomic GC operation before its reply. One world map, snapshot version, committer,
reachability rules, protected identities and evaluation accounting remain.

The integration passes 5,142 workspace tests/2 skips, including six new runtime
checks, plus doctests, formatting, warning-free Clippy/docs and debug/release
workspace builds. A production release run passes the same 596 inputs and 151
clean preloads with six fixtures/two address overrides. Distinct nil, preserved
player, 4,000 ms and normal 300-second GC stay unchanged. Collection reduces
898,442 cells to 15,264 with 656 roots; disposal of 32 chunks on two workers takes
154.1ms and total collector service is 201.3ms. This is one measurement, not a
controlled comparison with earlier samples or a new full replay/inventory
qualification. No Lima source or configuration changes are needed.
Evidence: /tmp/lpc-gc-workers-20261017.

The integrated driver subsequently passes the unchanged full retained gameplay
run: **2,160 gameplay inputs + 10 restored-favour inputs**, all **30 lifecycle
checkpoints**, both clean **151-preload boots** and normal exits in **487.60s**.
The exact mailbox sequence and account/body/bank/favour restoration checks pass.
Normal periodic GC runs during gameplay. Configuration, six fixture sources and
two address-only overrides stay unchanged. This replay precedes the container
source changes below; the whole-source inventory remains the earlier 966/1182.

The standard table and treasure chest now pass explicit float mass values while
preserving 64.0. The chest also initializes its default `in` relation before its
empty reset mapping. Both files compile. A focused production-driver run passes
**70 command inputs**, including both table relations and independent capacities,
full and closed refusals, normal put/get, open/close, visibility, poem reading,
empty reset and cleanup. All **151 preloads** pass and the process exits normally.
It uses stock leaflets as tokens, public weight controls and the existing six
fixtures, with no native code, configuration or authorization change. Fresh-driver
prop persistence and a combined full replay with these new inputs are not tested.

Two failed debug runs are retained separately: immediate shutdown during RST
scanning caused a background-task failure and committer-closed worker panic; a
repeat passed all object checks but hit two cold-compilation preload timeouts at
the unchanged 4,000ms limit. These driver findings are not fixed by the container
patch. The final optimized run waits for ordinary RST completion before shutdown
and has no Apply failure or worker panic. Evidence:
/tmp/lima-container-consumers-20261017.

The whiskey bottle spawned by Pinto's shadowy swamp now passes `2.0` to the
float-weight API. Its value range, bonuses, skill and salvage settings are retained.
Both the weapon and unchanged room compile. A focused production run passes
**27 new inputs plus all 70 retained container inputs**, with **151 clean preloads**,
ordinary RST completion and normal shutdown in **133.42s**. It checks the real room
spawn, look/get/wield/unwield/drop, integer values in 10..19 and reset restocking
without duplicate floor stock. The same six fixtures and two address substitutions
are used; no native code, configuration, framework or authorization changes.
Combat damage and fresh-driver weapon restoration are not tested. The full replay
and whole-source inventory above retain their earlier scopes. Evidence:
/tmp/lima-whiskey-bottle-20261017.

The weather daemon used by the example Weather Room now retains its timer ID,
cancels by ID and schedules the named callback. The **120-second period**, weather
selection, descriptions and recipient filtering are unchanged. All four daemon,
room and support targets compile. The production check retains 97 prior inputs
and adds **23 weather inputs**, passing all **120 inputs**, **151 clean preloads**,
ordinary RST completion and normal shutdown in **133.41s**. Repeated initialization
cancels old schedules and leaves one timer; a natural firing after 120 seconds
schedules one replacement. This sample remains foggy and emits no change notice;
changing natural ticks, all states and multiple recipients remain untested. No
native API, framework, configuration, authorization or fixture change is needed.
The full replay and source inventory retain their preceding scopes. Evidence:
/tmp/lima-weather-20261017.

The shared gettable module now registers its actual get/drop check functions.
Nested get validation keeps a container's gettable state separate from its
contents, recognizes the parser's individual and plural-array checks, and uses
the existing accessibility helper. Boolean, message and function responses are
verified through all four setters; direct moves respect drop hooks and setting
true restores get/drop behavior. The mailbox's original 62-input sequence remains.

Rust's conversation now invokes its named directory-giving callback, and the
nested workroom matches the open-handler signatures and validates the `open up`
alias through the same ownership check. The actual conversation gives one directory
with its workroom, retains per-body topic progress, and both open forms reach
Beek's room and NPC. Existing drop/put and unheld-open refusals remain.

All six real compositions compile. The full production replay passes **2,367
first-session inputs + 10 restored-favour inputs**, all **30 lifecycle checkpoints**,
both clean **151-preload boots** and normal exits in **336.70s**. It retains every
previous gameplay family and adds **41 workroom inputs** and **27 get/drop response
checks**. Account/body/bank/favour restoration, normal periodic GC and the natural
weather timer pass. Same six fixtures/two address substitutions and distinct nil/
preserved player/4000ms/300-second GC; no native code or authorization changes.
Earlier failed attempts and temporary diagnostics are retained as unqualified.
The standalone gettable module still requires its composition's `add_hook`;
no prototype extension is needed. Beek conversation and fresh-driver prop
persistence remain outside the check; the 966/1182 inventory is not repeated.
Evidence: /tmp/lima-workroom-20261017.

The loot chest matches inherited parser signatures, schedules its named expiry
callback and handles ownerless expiry. Explicit opening/get aliases and the
money/material put rules preserve its ownership and take-only checks. The shared
ordinary-item `get ... out of ...` handler delegates to the existing get-from
check; other object behavior is unchanged. Original headers, CRLF/LF endings,
capacities, text and timing formula remain.

**Eight complete targets compile**. The focused production run passes **95 new
loot inputs + 188 retained inputs**, **151 clean preloads** and normal shutdown
in **172.99s**. It checks player-owned, NPC-owned and ownerless construction,
ordinary opening, singular/plural taking through from/out-of/from-in forms,
wrong-relation and put refusals, rescheduling and removal. All three original
notices arrive naturally at approximately **15, 30 and 45 seconds**; the original
formula removes the chest at the third notice despite the constant's value of 60.
The retained real weather tick changes sun to snow and notifies the player.
Existing rule-failure selection can choose a generic no-object message for refused
aliases or money phrases; object states and the chest's checks are verified.

No native code/configuration or fixture changes. Loot-daemon generation and
placement, the death pipeline and fresh-driver loot persistence are outside this
run; the preceding full replay and 966/1182 inventory retain their measured scopes.
The next audit is the daemon that creates these chests. Failed controls remain
unqualified. Evidence: /tmp/lima-loot-chest-20261017.

The following loot-generation audit leaves source unchanged after commit
**ed33a625** (native notes **1e5f394a**). Of six targets, daemon/chest/coins compile;
gem and art fail on return/value types and quoted callbacks. The standalone boss
mixin lacks its host methods, and a tracked-source search finds no bundled host.
Both gem/art value getters also call themselves rather than the inherited getter;
this is source-reviewed, without executing recursion.

A **25-input** production audit catches both generator compile failures, verifies
coin creation/description and reproduces the daemon's unplaced chest. Level 17
produces no loot in the original table, isolating placement without random
selection or dependency substitutions. The chest is live, recipient-owned and
scheduled for its ordinary expiry, but has no environment or room membership.
All props and shell variables are cleaned up. **151 clean preloads**, normal
RST completion, saved account/body and normal shutdown pass in **133.09s**.
The two earlier harness-output failures remain unqualified.

Next repair the two generators and daemon placement, retaining existing float
value formulas, loot probabilities and boss rules. Type/callback changes are port
adapters; self-recursive getters and missing placement are present in the pinned
original source. The loot table only implements levels 0–16; extending it or
changing unused boss policy is separate work. No actual boss combat, random
distribution or loot persistence qualification is claimed. Native source/config,
fixtures, distinct nil/preserved player/4000ms and 300-second GC are unchanged.
The preceding full replay and 966/1182 inventory retain their measured scopes.
Evidence: /tmp/lima-loot-generation-audit-20261017.

The audited three-file repair is applied after notes commit **0dfba629** (native
**72802a4f**). Gem/art query their inherited value getters, adapt return/value
types and schedule named callbacks. The missing maximum gem roll returns category
0 explicitly; existing thresholds, formulas, text and delays remain. The daemon
resolves its recipient first and places the chest in that recipient's room before
moving loot inside. Loot bands, boss rules and expiry are unchanged. Headers, LF
and original final-newline states remain; no native code/config or fixture edits.

**Seven complete targets compile**. Production passes **62 new + 283 retained
inputs**, **151 clean preloads**, normal documentation scanning, saved account/body
and normal shutdown in **177.45s**. Natural callbacks generate four gems across
caps 10/50/64 and four art objects across both existing tiers. Inherited value
factors, original value/mass ranges, distinct clones, valid index samples, stable
repeated gem values and ordinary look/get/drop work. Nil and non-player recipients
use the existing body fallback. Level 17 retains empty generation but now places
its chest; a supported level 16 run produces one coin pile and five gems in a
placed, owned chest. Ordinary opening and coin collection work, and the wallet
and all props are cleaned up. Art is checked through its public factories.

The unchanged six retained helpers keep all container/bottle/workroom/gettable/
weather/chest assertions. Real weather changes fog to sun; all three original
chest-expiry notices arrive naturally at 15/30/45 seconds. Actual boss combat,
random distributions and loot persistence remain outside this run. The earlier
full replay and 966/1182 inventory retain their measured scopes. The inventory above now measures
these leaf ports and selects the next complete consumer.
Evidence: /tmp/lima-loot-generation-20261017.

Inventory notes are committed as **2ffc14ad** (native **f96aedc2**). The following
Rocky Beach repair changes two LPC files: debris converts its unchanged mass 5000
to float; Outside_Cave registers bare obj_arrived, uses the item's normal remove
path and checks mass in the configured mass build. The original size branch is
preserved, untested. Stock rock settings, thresholds, text, quest rules and reset
policy remain. Headers/LF/final newlines remain; no native code/master/config or
fixture edits. Prior failed controls distinguish a scenery-count assertion,
inactive callback, rejected direct destruct and the unavailable legacy size getter.

**Five complete targets compile**. Production passes **48 new + 345 retained
inputs**, **151 clean preloads**, normal documentation scanning, saved account/body
and normal shutdown in **176.75s**. Real Outside_Cave/east/Rocky_Beach construction,
debris/ocean spawns, repeated distinct rocks, ordinary look/get/drop, direction
refusals, stable reset and missing-debris replenishment pass. The original pile
stays during pickup; small rocks sink at the cave entrance while the large pile
splashes and remains. Both ocean-object and bare wade routes return correctly.
The existing ocean hook starts the pirate quest; full quest completion/combat
are outside this check. Seven unchanged helpers retain the generator/container/
bottle/workroom/weather/gettable/chest assertions, including natural weather and
all three chest-expiry notices. Previous full replay and 985/1182 inventory retain
their scopes; no additive recount. Evidence: /tmp/lima-rocky-beach-20261017.

Rocky Beach support is committed as **7d47e4af** (native **684e8a1a**). The next
guild-guard adapter changes only `std/guild_guard.c`: use the existing block-action
callback instead of calling a private parent handler. Its explicit allow result
matches that API's polarity. Members/allies pass; other characters receive the
original refusal text and action. The blocking module, native privacy, master,
configuration, fixtures, constructors and headers/LF/final newline are unchanged.

**Three complete targets compile**. Production passes **63 new + 393 retained
inputs**, **151 clean preloads**, normal documentation scanning, account/body saves
and normal shutdown in **178.76s**. The actual Monster_Room factory creates its
Yakitori guard. Nonmembers and zero-level memberships are blocked; members pass.
Temporary guild records and a clone of the stock room test ally membership,
unrelated membership and revocation without changing bundled ally relationships.
Movement transfers the guard's hook; reset preserves one live guard and replaces
one removed guard, with stale hooks pruned normally. Memberships, shell path,
temporary catalog entries and props restore. Eight unchanged helpers retain all
previous generator/beach/container/bottle/workroom/weather/gettable/chest checks.
No combat or saved-guard restoration claim; prior full replay and 985/1182 inventory
keep their measured scopes. Evidence: /tmp/lima-guild-guard-20261017.

Guild-guard source/notes are committed as Lima **686d0a4d**, native **9f1ea8b2**.
The constructor coverage map now accounts for all **34 standard-room sources**:
**30 concrete rooms**, plus **four templates/servers**. Nine concrete rooms and all
four templates/servers retain source-matched prior evidence. The remaining **21
room checks pass**, including direct stock and nested adventurer equipment/table
shovel. Sixteen rooms load directly, two through checked dependencies; three are
already loaded during startup. **67 focused inputs**, **151 clean preloads**, normal
RST scanning, account/body saves and clean shutdown pass in **153.59s**.
No LPC/native/configuration/fixture changes. Construction coverage does not qualify
all gameplay/exits; the prior full replay and 985/1182 inventory keep their scopes.
Evidence: /tmp/lima-room-construction-20261017. Constructor notes are committed as
Lima **46327624**, native **2ac8e2ea**; do not repeat the completed constructors.

The Lava Room audit is committed as Lima **4313c3b3**, native **320664db**.
The one-file adapter now registers bare `when_person_enters` and uses `o->remove()`
with an immediate return after the original ordinary-object melting message.
Native/master rules, timer/vehicle policy, configuration and fixtures are unchanged.

**One complete target compiles**; **42 production inputs**, **151 clean preloads**
and normal RST/saves/logout/shutdown pass in **140.68s**. The actual production
hook runs without replacement or expected errors. Repeated direct object arrivals
and ordinary `drop` remove rocks without extra timers/leftovers. Natural five-second
burns deal 5 damage and reschedule; departure consumes the pending timer without
more damage. A ridden Camaro protects its rider; existing cosmetic vehicle burns
and quick-reentry duplicate timers are preserved. Health/healing, path and props
restore; the fixed callback remains installed. Evidence:
/tmp/lima-lava-room-20261017. Prior audit, constructor/full replay and source
inventory retain their scopes; there is no additive recount or repeated Rust gate.

Book audit notes are committed as Lima **f61c59cd**, native **e9d53ae7**.
Only `std/book.c` changes: remove dangling unused open/close hooks, use named
callbacks/int page arrays/float mass, preserve selected pages, resolve aliases to
canonical page names, fix backward/numeric-append behavior, retain title/author,
and handle empty/untitled books. The trailing backwards form follows validation.
Public page/readable signatures and trivial open/close responses stay unchanged.
No native/shared-readable/guide/parser/master/configuration/fixture edit.

**Two complete targets compile**. **74 production inputs**, **151 clean preloads**
and normal RST/saves/logout/shutdown complete in **133.69s**. Core book commands,
contents/page removal and fallback, append beyond page ten, empty-book append and
resident callable-page reading pass without diagnostic source or hook controls.
A real book-owned callback resolves the prior audit's invalid callback-owner probe.
Props and path restore. Evidence: /tmp/lima-book-20261017.

Book source/notes are committed as Lima **a5efeef1**, native **c2895b85**; routing
audit notes as Lima **d0063df1**, native **1d138a6d**. Only the read/turn rule arrays
now change: qualified about forms precede broad about STR, and page-turn forms
precede bare OBJ. All rule texts and handlers stay. Native first-success and
equal-progress failure ordering are unchanged.

Both complete verb targets compile. **32 routing + 74 retained book inputs** pass
in **140.45s**, with **151 clean preloads**, normal RST/saves/logout/shutdown.
Actual registry order, qualified about in/from reads, the four ordinary turn error
messages, valid turns, page preservation and cleanup pass. Numeric/alias/empty/
resident-callback book coverage remains green. No runtime rule or hook control.
Evidence: /tmp/lima-command-routing-20261017.

Rejected trailing-direction turns retain the missing-noun diagnostic tie; valid
trailing turns and refusal state preservation pass. Native/parser, shared-readable,
book, master, configuration and fixtures are unchanged. Previous temporary audit:
/tmp/lima-command-routing-audit-20261017. Prior inventory/full replay/constructor
counts retain their scopes. Read/turn source and notes are committed as Lima
**d234149d**, native **b47b39ed**.

Pelt audit notes are committed as Lima **a99a39c1**, native **2023c7af**. Only
std/pelt.c now changes: float constructor mass, initialized size/salvage level,
direct save registration, safe no-argument construction, saved current mass and
restored type IDs. Constructor mass=s and setter mass=0.5*s remain unchanged.

**Four complete targets compile**. **55 production inputs**, **151 clean preloads**
and normal RST/saves/logout/shutdown pass in **139.90s**, with no diagnostic
source/hook control or expected error. Ordinary look/get/drop, generic IDs, exact
size/type/custom-mass restoration through no-argument and recursive inventory loads,
zero/positive salvage paths, real loot/corpse placement and collection pass. Positive
yield is bounded rather than fixed; no probability claim. Shared save/object/loot/
crafting/corpse, native/configuration and six existing fixtures are unchanged.
Evidence: /tmp/lima-pelt-20261017. Prior diagnostic audit and incomplete attempts:
/tmp/lima-pelt-audit-20261017. No fresh-driver pelt restore or alternate-size claim.
Previous inventory/full replay counts retain scope. Pelt source and notes are
committed as Lima **fd952bd6**, native **c047e1a0**.

Meat audit notes are committed as Lima **6a0aa954**, native **1ab3938c**. Only
std/meat.c now changes: compatible variadic construction, the existing healing host
contract, explicit junk/edible/decay setup, direct type/level/mass save registration,
and separate initial rolls from derived identity/actions. Public setup_meat retains
its reset behavior. Load rebuilds derived state without resetting saved counts;
five-minute configuration precedes deferred state registration.

**Four complete targets compile**. **66 production inputs**, **151 clean preloads**
and normal RST/saves/logout/shutdown pass in **133.50s**, without diagnostic
source/hook controls or expected failure. Real partial eating leaves six of seven;
direct/no-argument/recursive inventory restoration retains those counts, type/level,
poison data, mass 0.375, remaining decays and begun state after callbacks complete.
Type IDs/descriptions restore without a manual child hook. Ordinary commands,
explicit public setup reset, actual state-daemon decay through removal and stock
badger meat collection/eating pass. No forced loot selection or probability claim.
Poison is only checked as saved data and cleared before eating. Shared/native/
configuration and six fixtures are unchanged. CRLF/no-final-newline format remains.
Evidence: /tmp/lima-meat-20261017; previous audit /tmp/lima-meat-audit-20261017.
No fresh-driver meat restoration or alternate build claim; previous inventory/full
replay counts retain scope. Meat source/notes are committed as Lima **b24a3d7a**,
native **59fb3ec6**.

Ladder audit notes are committed as Lima **78e849ee**, native **32a7e9f9**. Only
std/ladder.c changes: remove the unused third more_create formal to match its parent.
Function body, parent attachment, destination forwarding and base callback stay.

**All six complete targets compile**. **58 production inputs**, **151 clean preloads**
and normal scan/saves/logout/shutdown pass in **136.43s**, without diagnostic
source/hook controls or expected failures. Actual climb/ascend/descend movement,
two-way relative and one-way object/string destinations, singular identity,
attachment, relocation base, get refusal and unconfigured defaults pass. Props,
path and variables restore; stock stairs remain. No shared/native/config/fixture
change. Evidence: /tmp/lima-ladder-20261017; prior audit:
/tmp/lima-ladder-audit-20261017. Prior room/navigation, inventory and full replay
scopes remain. Ladder source/notes are committed as Lima **fb1de80e**, native
**860a3950**.

Climb-challenge audit notes are committed as Lima **3f3b929e**, native **b589f1cb**.
Only std/climb_challenge.c changes: integer refusal, direction descriptions,
variadic forwarding, direction-based reflex checks and default/validated failure
messages. Existing injury/skill/reflex cost and traversal policy are preserved.

**Six complete targets compile**. **144 production inputs**, **151 clean preloads**
and normal RST/saves/logout/shutdown pass in **133.77s**. Accepted parser command
forms now share reflex rules; actual success/failure movement and bounded existing
injury, default/custom messages, configuration rejection and no-route cases pass.
Health/skills/ranks/stat/reflex/rate/path/props restore through actual getters.
No source controls or constructor-failure fixture; the six prior fixtures suffice.

Parsed go up/down object forms retain the outer go verb's refusal. The class's go
alias is separately tested through direct calls and follows the same reflex policy;
no parsed-go travel is claimed. Shared/native/configuration behavior is unchanged.
Evidence: /tmp/lima-climb-challenge-20261017; prior diagnostic audit:
/tmp/lima-climb-challenge-audit-20261017. The refreshed source inventory is recorded
above, preserving earlier functional scopes and host classifications.

The digging audit at native **ad86b192** and Lima **01bc51ad** reproduces a nested
prompt failure in M_DIGGER/M_DIGGABLE: pop-after-forward removes a newly created
prompt, leaving the old callback with a cleared user binding. Source remains
unchanged pending adoption. A two-module temporary proposal uses existing
modal_simple() and this_user()->modal_pass(input), requiring no driver change.

**84 proposal inputs**, **151 clean preloads** and normal scan/saves/logout/
shutdown pass in **134.03s**. Real stock pickup, explicit/inferred/reversed
forms, hole growth/exhaustion, castle refusal, both selection prompts, ordinary
pass-through, both nested handoffs and same-object repeated prompts pass. Light,
props, path, stock, inventory and unchanged quest/XP/score restore. No handler
recovery is needed; same six fixtures and two loopback-address overrides.

The sand's chest message has no chest-construction implementation. The missing
quest prerequisite prevents reward; positive reward/full quest remain separate.
The existing omitted tool argument is unused by both targets and is not the
observed lifecycle failure. Prior setup/capture/ID mistakes and a discarded
modal_remove proposal are retained as harness history, not final qualification.
Evidence and proposed patch: /tmp/lima-digging-audit-20261017. Recommend adopting
these two callers with retained replay, then the button/secret-door/quest chain.
Eight unchanged complete compile passes are retained from the matched inventory;
no fresh source recount, Rust gate, native API or dedicated-source change is claimed.

The digging prompt repair adopts only std/modules/m_digger.c and m_diggable.c,
matching the reviewed proposal. Existing modal_simple() handles the one-shot
selection; this_user()->modal_pass(input) forwards other commands without popping
a replacement prompt. Audit notes are committed as Lima **1ef5c2bb**, native
**842fd5ed**; source adoption and these notes are pending commit.

**Eight complete targets compile**. **84 retained production inputs** pass in
**136.42s**, with **151 clean preloads** and normal scan/saves/logout/shutdown.
Input lines and observed states match the proposal run; there are no proposal source
overrides or handler-recovery commands. Both nested handoffs and same-object repeated
prompts, ordinary selections/pass-through, digging forms, hole stages and refusals
pass. Light, stock, props, inventory, path and unchanged quest/XP/score restore.
Same six fixtures and two loopback-address substitutions; no native/config/binary
change, new Rust gate, whole-source inventory or room-construction rerun.

Chest construction and positive pirate-quest reward remain separate; the unused
tool argument stays. Evidence: /tmp/lima-digging-20261017. After committing this
adoption, audit the Navigation Room button, linked secret doors and remaining
pirate milestone integration.

Digging source/notes are committed as Lima **133993b7**, native **76052e17**.
The subsequent passage audit finds one existing example error: rock_wall schedules
close(), which does not exist. Button press/push open both walls but leave them
open after eight seconds. The dock/river counterpart's do_close callback closes
both normally; native timer and shared door behavior need no change.

Ten complete targets compile. A one-line temporary proposal uses bare do_close
at the same eight-second delay. **67 proposal inputs / 134.05s**, **151
clean preloads** and normal scan/saves/logout/shutdown pass. Real callback timing,
both movement directions, reciprocal visibility/state, already-open behavior and
cleanup work without a manual close reset. Original timing failures are recorded
separately, not presented as passing timer checks.

Focused ordinary inputs earn begin10/100XP, secretPsg15/150 and end25/250, with
no injected progress; repeated actions do not add rewards. The character finishes
with earned progress and normal saves complete. Props/light/path/inventory restore;
the table reset is requested.
The older six-test continuous cave route is retained as historical evidence; it is
not rerun. Saved quest restoration after restart remains separate.

Dedicated source is unchanged pending adoption. Six fixtures and two loopback
address substitutions remain; the only proposal source replacement is rock_wall.c.
The stale constructor destination in Small_Dock is unused; actual door destinations
are reciprocal, so it is not changed. No Rust/full-inventory/room/framework rerun.
Evidence and one-line patch: /tmp/lima-pirate-passage-audit-20261017. Adopt and replay
that callback change, then verify completed quest restoration. Chest creation stays
outside the repair.

Passage audit notes are committed as Lima **8eacce39**, native **d8f2b752**.
The one-line rock_wall.c repair is now adopted: its eight-second callback uses
bare do_close. Source and updated notes are pending commit; the implementation
matches the reviewed proposal exactly.

**Ten complete compile targets and 67 retained production inputs pass**, the latter
in **135.85s**, with **151 clean preloads** and normal scan/saves/logout/shutdown.
Input lines and observed states match the proposal run. Press/push naturally close
both walls without proposal overrides, manual callback advancement or close resets.
Door state/visibility/movement, refusal paths and ordinary quest milestones work;
the character finishes with 25 points/250 XP and normal saving completes. A fresh-
process quest restore is not yet checked. Cleanup retains the preceding audit scope.

Same six fixtures, two address substitutions and configuration; no native/binary
change, Rust gate, full source sweep or continuous cave route rerun. Evidence:
/tmp/lima-pirate-passage-20261017. Commit this repair, then verify saved quest state
and points/XP after restart. Chest construction remains outside this fix.

The passage repair is committed as Lima **a4ef8318**, native **62a4b21e**.
Completed quest restoration is now verified without source changes: **27 earning/
save inputs plus eight restored inputs in 137.36s**, across **two separate
driver processes**, both with 151 clean preloads and normal saved login/logout/
shutdown. The same six fixtures and two address substitutions remain.

Explicit save at secretPsg contains 15 points/150 XP; completed saves contain end,
25 points and 250 XP. Nine archived body-save checkpoints verify quests/points/experience.
The completed file stays byte-identical through restart authentication until play;
the restored body's mapping, finished status, identity/link, points/XP, inventory,
light and location match. Quests, score and re-saving preserve that state.

The quest module's update calls add_experience, which saves a linked body. Thus
completion is already written before logout; the earlier inference that it relied
only on explicit save/logout was incomplete. Existing policy is unchanged. No save
file or quest progress is injected. The prior continuous route and timer results
retain their scopes, with earning used only as restoration setup. First RST scan
and its cached restart watermark are preserved; no scan-cleanup fixture is added.
Evidence: /tmp/lima-quest-restoration-20261017. Pirate passage/quest work is closed;
return to the classified source backlog and actual consumers. No new Rust gate,
full inventory, chest implementation or native/configuration/binary change.

Quest restoration notes are committed as Lima **0caed83c**, native **cae150fa**.
The remaining-source review adds 28 confirmed compositions: 13 selected adversary,
seven behaviour-tree and eight object-module fragments. **43 scoped checks** show
11 complete hosts passing, 30 expected isolated rejections and two failing tools
with source callers. No unexpected outcome, timeout or crash remains.

Against the preceding 188-file failure set, **110 now have reviewed passing hosts**
and **78 remain for review**: 14 unselected combat/statistics alternatives, 32
WWW/contrib/attic files grouped only by location, two deferred/disabled commands,
two tools with confirmed callers and 28 further source/host cases. Directory
location does not establish activation, and this is not a count of native gaps.
No new whole-source measurement or runtime qualification is claimed.

That review selected obj/mudlib/aliasmenu.c. Both configured shells inherit SHELL;
the normal alias command loads this file for bare alias and alias-name display.
Its inline function syntax still fails compilation. The secure/cgi/webed wrapper
also reaches failing obj/secure/webed, without a new HTTP execution claim.
Evidence: /tmp/lima-compile-classification-20261017. No source, native API, binary,
profile or fixture changes; completed pirate/quest/equipment/framework work stays
closed. The audit below now covers the selected shell/API/persistence path.

The alias-menu audit uses a temporary one-file proposal: named native callbacks,
nil/string defaults and a retained menu handler, plus corrections to shortcut
commands, numbered completions and editing space-omission flags. Five of seven
compile targets pass; the other two are expected isolated M_ALIAS/SHELL host parts.
Both complete shells and the user compile.

**83 first-session inputs + 13 restored inputs / 142.92s** complete over two driver
processes, each with 151 clean preloads and normal saves/logout/shutdown. Custom class records,
defaults and xverb flags survive authentic save/login/restart and are cleaned up.
Two unresolved findings are explicitly asserted: body.quit's blind modal_pop on
!quit leaves an editor clone, and M_ALIAS's obsolete sizeof(class) != 4 migration
resets global defaults to personal aliases in saves. This is not a clean lifecycle
pass. Evidence: /tmp/lima-alias-menu-audit-20261017.

The adoption below covers the menu adapter and obsolete migration; quit notification
remains separate. That audit changed no persistent mudlib/native source, profile,
binary, save format, shell API or access policy. No class-size reflection is required.
The old whole-source count is not recomputed.

The dedicated source now contains that exact menu adapter and removes the obsolete
class-size migration from M_ALIAS. Five complete/definition targets pass; two
isolated host-part failures remain expected. **83 first-session + 13 restored inputs
/ 142.96s** pass the adapter checks across two clean 151-preload driver boots.
Custom aliases/defaults/xverbs restore exactly. Six save checkpoints exclude globals
colors/p and restored flags remain 1/2; supplied zero-status defaults and existing
personal overrides are preserved. This does not repair earlier accidental overrides.

Runtime uses the dedicated sources without alias proposal overrides; the same six
fixtures and two existing loopback address substitutions remain. Normal command
lines match the audit, with stronger save/metadata assertions. The known !quit editor
leak was the sole remaining finding; the focused cleanup below resolves it.
Evidence: /tmp/lima-alias-menu-20261017. No native/config/binary/format/API change,
new full-source count, stock data rewrite or clean-cancellation claim is made.

Alias source/notes are committed as Lima **bcd959bf**, native **0ad959a1**.
The focused quit fix changes only secure/user.c and std/body.c: the body now invokes
a current-body-only entry point that reuses the existing clear_input_stack loop,
removing handlers before sending cancellation. Ordinary modal completion is unchanged.

**8/8 compile targets and 92+13 runtime inputs / 154.16s** pass across two
clean 151-preload boots and authentic save/login/restart. Pending and nested editors
are destroyed; repeated quit/re-entry returns to one shell handler. Existing alias
classes/defaults/xverbs/global flags remain intact, six save checkpoints preserve
the filter, and no caught cancellation error occurs. This closes the known alias
quit leak. Evidence: /tmp/lima-quit-input-20261017. No native/profile/binary/format
change, new fixture, broad handler qualification or whole-source recount is made.

Quit source/notes are committed as Lima **55deab9c**, native **e4a6c18c**.
The next consumer review finds webed behind the optional HTTP daemon, commented
out in the configured preload. Its first compile failure comes from audit.h's
error->throw macro expanding the display_error parameter named error; a minimal
declaration confirms the collision and passes when renamed. Existing mappingp and
optional CGI-header adaptations are retained. The complete editor remains deferred.

Nine source checks show eight passes and that expected editor rejection. Wrapper,
HTTP, M_ACCESS, master, drop, COINS, netstat and full simuls compile. Three small
compile examples verify macro causality and the unselected USE_MONEY branch.
An initial reference scan of 28 remaining source/host cases finds MONEY unselected
and netstat inspecting already-loaded legacy sockets. Negative reference matches
do not establish inactivity; relative inheritance and object-stock review are next.
Evidence: /tmp/lima-web-editor-audit-20261017. Source/config/binaries remain unchanged;
no runtime/HTTP test, service activation or new whole-source count is made.

Optional-editor notes are committed as Lima **a22e3aed**, native **5343469c**.
The 28 further source/host entries now have consumer decisions. **47 scoped checks**
give 19 selected/reference passes and 28 retained target failures, with no timeout
or crash. Four entries are discovered by unittest items (three weapon leaves plus
M_SPECIAL_WEAPON); one belongs to the relative stone/sword sample, one to legacy
service/inspection, one to unselected MONEY, five to sample/attic chains, and 16
have no configured caller identified. The last group is not declared unreachable.

The item suite glob discovers baseball_bat, rapier and special_longsword; its
registered clone helpers prefix /domains/std/. The last weapon inherits the
special-weapon mixin. Parent providers and the suite/daemon/command compile, but
this source review does not execute or qualify the suite's callbacks. Next audit
that invocation/clone boundary and those leaf adapters, preserving established
equipment APIs and previous gameplay coverage. Evidence:
/tmp/lima-consumer-classification-20261017. Sources/config/binaries stay unchanged;
no new full-source count, gameplay result or native requirement is established.

Consumer-classification notes are committed as Lima **54555bda**, native **b831bdfb**.
The item suite first aborts on direct destruction of its troll's starting sword;
its teardown has the same ownership-policy rejection. Only std/tests/items.c now
changes to use the objects' existing remove methods and remove test inventory
before its actor. The tested one-file proposal was adopted byte for byte afterward.
Master, framework API, callback array payload and weapon sources are unchanged.

The real command completes **26 cases: 22 pass, four fail**. Setup, weapon/armour
checks and cleanup pass, with no suite actor or inventory residue. Array and scalar
clone controls both pass. **37 inputs / 146.26s**, 151 clean preloads and normal
saves/logout/shutdown verify the proposal; 11 compile targets retain seven passes
and four known rejections before/after. This is not a complete item-suite pass.

Remaining clones are baseball_bat, rapier and special_longsword (compile failures),
plus stick, whose typed colour constructor is called without fixture data and logs
an expected init-program error. Next adapt the three weapon sources/mixin and give
the stick fixture a valid colour. Evidence: /tmp/lima-item-suite-audit-20261017.
The original control also retains a later, corrected harness pwd assertion; its
source-failure observations are not relabelled as a clean run. No native/profile/
binary/privilege change, new fixture, full inventory or broad gameplay claim.

The ownership adapter is committed as Lima **f9bde8b2**, native **f00de4cd**. The
four remaining item construction cases above are now resolved by four source-file
adapters: float setter arguments, native special-weapon callbacks/class access/save
registration and a float conditional, and a red colour for the typed stick fixture.
Test framework, equipment APIs, master policy and special-item selection remain.

The real item command passes **26/26** with no suite residue. A separate special
longsword's actual scheduled roll, five saved fields and four canonical event cases
pass; a blue stick retains its constructor data. Both controls remove themselves.
**14/14** scoped real-source checks and one temporary event fixture compile.
**63 inputs / 134.21s** pass with 49 positive individual loads (21 crafting recipes),
151 clean preloads, normal saves/logout/shutdown and no runtime error. The actual
dedicated-source copy uses the prior six fixtures/two loopback substitutions plus
one temporary event helper, with no proposal source override. This prepared-runtime
result adds no cold-performance, full-source or other-suite/all/all quiet claim.
Evidence: /tmp/lima-item-construction-20261017. Native/profile/binaries stay unchanged.

The item construction changes are committed as Lima **f3a16824**, native **dc40cf01**.
The bundled liquids suite passes its first six assertions, then direct actor
destruction is refused by the existing master policy. Its one-file teardown adapter
asks captured deep inventory, actor and keg to remove themselves, with nil guards.
The real command now passes **7/7**; cleanup also removes ale still inside a filled
bottle and safely repeats. No fluid implementation or assertion changes are needed.

**24 inputs / 139.21s**, 151 clean preloads and normal saves/logout/shutdown
pass. Twelve scoped proposal/control checks and the original-suite compile check
pass. The temporary runtime switches only the suite source, then those tested bytes
are adopted exactly. The prior six fixtures/two loopback substitutions remain.
Player/lab/clone state returns to baseline; no unexpected error or residue remains.
Other suites/all/all quiet and cold-suite performance retain their prior scope.
Evidence: /tmp/lima-liquids-suite-20261017. Native/profile/binaries stay unchanged.

The liquids-suite fix is committed as Lima **23f6497e**, native **5e62be06**.
The race suite loads shared blueprints; its original cleanup is refused for all
seven, yielding **4/5**. Its one-file adapter clears the suite's race references
and example selection while leaving the shared objects loaded. The real command
now passes **5/5**, and repeated cleanup succeeds. All seven blueprint identities,
the active human character and existing race clone groups remain unchanged.

Thirteen scoped proposal/control checks and the original-suite compile pass.
**21 inputs / 133.28s**, 151 clean preloads, normal saves/logout/shutdown
and no unexpected errors pass. The tested source bytes are adopted exactly after
the disposable run. The prior six fixtures/two loopback substitutions remain;
its temporary shell identity marker is removed. Master, race content, assertions,
framework and native/profile/binaries remain unchanged. Limbs, salvage, all/all quiet
and cold-suite performance retain their prior scope. Evidence:
/tmp/lima-race-suite-20261018.

The race-suite fix is committed as Lima **d39f288c**, native **66bd5b52**. The limbs
suite needs three local adaptations: test objects remove themselves, the wield
check requires exactly one occupied limb instead of comparing a nullable array
with zero, and both unwield callbacks select the cloned longsword's actual ID.
Its sixteen cases and expected outcomes, equipment implementations, framework and master remain.

The original setup is refused before counting a case. Ownership-only reaches 9/16;
the wield check reaches 10/16, exposing the selector mismatch and cascading failures.
The corrected real suite passes **16/16**, including both expected refusals. Explicit
item-state controls confirm one/two-hand wielding, unwielding, occupied-hand refusal,
cleanup and repeated cleanup; player/lab/clone state returns to baseline.

Sixteen scoped source targets compile, with the corrected final suite recompiled;
original and ownership-only suite checks also pass. **22 inputs / 139.25s**,
151 clean preloads and normal saves/logout/shutdown pass without unexpected errors.
The diagnostic run and its non-green results remain separately recorded. Tested
final bytes are adopted exactly afterward. The prior six fixtures/two loopback
substitutions remain; no new helper or native/profile/binary change is introduced.
Salvage, all/all quiet and cold-suite performance retain their prior scope. Evidence:
/tmp/lima-limbs-suite-20261018.

The limbs-suite fix is committed as Lima **c325475f**, native **f87b40af**. The
salvage suite now checks actual sword removal, tracks only base-material clones
created by its own command, and asks outputs/inventory/actor to remove themselves.
Its troll has no material pouch. The shared material prototype and existing clones
are excluded; command errors propagate after capturing outputs. The five cases and
expected outcomes, framework/master and crafting/material implementations remain.

The original reaches 3 passes/1 failure before teardown is refused; ownership-only
completes 4/5 but leaves three material objects in this run. The final real suite
passes **5/5**. A fixed-yield control proves refusal leaves the wielded sword intact
with no yield, then unwielded salvage removes it and creates two steel scraps.
Natural/repeated cleanup removes both while preserving an unrelated wood scrap;
the player's pouch and tracked player/lab/clone state return to baseline.

Eleven scoped source targets compile, with the final suite recompiled; original and
ownership-only suite checks also pass. **34 inputs / 133.83s**, 151 clean
preloads and normal saves/logout/shutdown pass without unexpected errors. Tested
bytes are adopted exactly afterward. The prior six fixtures/two loopback substitutions
remain; no new helper or native/profile/binary change is introduced. All five shipped
suites have individual results; all/all quiet still needs combined qualification.
Material probabilities, arbitrary recipe yields, player-pouch credit and cold-suite
performance retain their prior scope. Evidence: /tmp/lima-salvage-suite-20261018.

The salvage-suite fix is committed as Lima **52271b44**, native **70f1812e**. Both
combined commands now pass **59/59** through the real command/daemon: `unittest all`
and `unittest all quiet`. Only the five shipped suites are discovered, with observed
counts 26/16/7/5/5 and matching aggregate fields/output. Normal mode prints 59 case
results and five summaries; quiet prints only final totals. Each run replaces all
five suite objects, while seven shared race blueprints and tracked player/lab/clone
state remain unchanged. Identity markers are removed; following look/pwd work.

**55 inputs / 138.44s**, 42 individual program loads, 151 clean preloads and normal
saves/logout/shutdown pass without unexpected errors. No extra LPC fixture or gameplay
replacement is installed; the two existing loopback substitutions remain. No further
implementation/configuration/binary change is needed. This prepared-runtime result
adds no cold-suite performance or full-source inventory claim. Evidence:
/tmp/lima-bundled-tests-20261018. The next check is a fresh bounded source inventory
with existing host/optional/legacy classifications carried forward.

The combined-test notes are committed as Lima **fb3ad0a9**, native **45648cfb**.
A fresh full inventory measures **998/1182 compiling, 184 failing, zero regressions,
timeouts or crashes in 245.587s**. All 994 previous passes remain and the target set
is unchanged. Four gains are baseball_bat, rapier, special_longsword and aliasmenu.
Fifteen .c files changed since the previous full sweep; headers/profile/binaries match.

**111** failed standalone fragments have freshly passing real hosts, including the
newly confirmed special-weapon composition. Changed alias/user/body inputs retain
the reviewed inheritance/provider contracts. The other **73** retain their source/
profile/consumer contexts; they are not 73 native feature requests. All 17 core
objects and the previous command success groups remain green. The digging fragments'
first errors now name their already-reviewed modal_simple provider; actual hosts pass.

Two bounded CLI processes compile without target constructors or driver/Cargo
overlap. Source/configuration/binary guards pass; no implementation changes or new
runtime claim are added. Combined normal/quiet 59/59 results retain their preceding
scope. Evidence: /tmp/lima-source-inventory-20261018. The next concrete optional
consumer is the on-demand stone/sword example, whose relative child retains its
integer-mass blocker; constructor/get/release behavior needs its own audit.

The inventory notes are committed as Lima **4910e637**, native **33ca97b6**. The
on-demand stone/sword sample now adapts only its child: explicit float mass and
set_id("sword"), matching the parent's acceptance check. Original construction
reproduces the type rejection; mass-only compiles but the child advertises only
weapon and placement fails. The stone/king-choice/container implementations stay.

Eight complete proposal/control targets and two mass-only targets compile. Two
connected real players verify selection, nonking refusal, king extraction, return
and re-extraction of the same sword, non-sword rejection and forced child cleanup.
Mass remains MEDIUM=16.0 and weapon class 25. Global/per-relation hide behavior is
preserved, including the global flag remaining set after return. Guest permissions
stay unchanged; observations use the administrator session and guest actions are
ordinary commands. Two earlier partial harness runs remain separately recorded.

**36 shell inputs plus 15 guest-creation inputs / 132.78s**, 151 clean
preloads, both account/body saves and normal logout/shutdown pass without unexpected
errors. Tracked player/inventory/pouch/lab/clone state restores. Tested child bytes
are adopted exactly afterward; no new helper or native/profile/binary change.
This sample is on-demand, with no selected stock/preload root established. No new
full-source total or king-logout reselection claim. Evidence:
/tmp/lima-stone-sword-20261018.

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
