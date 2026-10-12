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

Full inventory, wizard/player-command and startup results retain their preceding
scope.

Current compatibility and handoff notes are in the neighboring lpc-rs checkout:
`doc/driver-comparison.md` and `local/lima/compatibility.md`. Temporary test
harnesses and execution logs belong in `/tmp/`. Continue source changes in this
checkout rather than making another source copy under `local/`.
