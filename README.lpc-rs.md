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

The retained player checks compile 65 of 67 player commands and all 12 supporting
targets. Mudlist passes 13 runtime cases using both actual shells. `su` passes
refresh, race-change and password-modal checks; its existing account/character
lookup limitation is recorded in the port notes. Experimental `random2` remains
deferred and `suicide` is disabled upstream.

Wizard directory navigation passes seven cases covering 28 inputs, including
saved directory history and current-file restoration after shell recreation.
Focused checks compile 15 of 81 wizard commands, 18 of 49 transitional commands
and all 9 navigation support targets. The existing root-origin `cd -` edge is
recorded separately in the port notes.

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

Full inventory, wizard/player-command and startup results retain their preceding
scope.

Current compatibility and handoff notes are in the neighboring lpc-rs checkout:
`doc/driver-comparison.md` and `local/lima/compatibility.md`. Temporary test
harnesses and execution logs belong in `/tmp/`. Continue source changes in this
checkout rather than making another source copy under `local/`.
