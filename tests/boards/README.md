# Portable-board regression tests

After building the sibling lpc-rs workspace, run from this checkout:

```sh
python3 tests/boards/test_boards.py
```

Set `LPC_RS_COMPILER` and `LPC_RS_DRIVER` to choose other built binaries. The
standard-library Python runner copies the actual library to a temporary directory,
uses POSIX resource limits, compiles eight complete programs and runs **54 LPC
assertions**. It retains distinct nil, preserved call-out players and the 4000ms
limit. The driver listens only on loopback port zero, without clients or external
service connections, and the temporary files are removed afterward.

The fixture calls the actual portable/provider APIs and uses the real news daemon,
editor, user news/mail modules, both standard and Mh mail handlers, mailbox and
mail daemon. A recording user supplies the modal boundary, body flags and output;
its `save_me` counts news-save calls rather than persisting a real account. The
master permits local file operations. This is not a parser-dispatch, full-login,
real-master permission-policy or upstream-driver test.

Assertions cover empty and removed articles, numeric bounds, visible numbering,
out-of-order and shared read sets, live descriptions, explicit board mass,
subject prompting/retry, plain and empty posts, cancellation, captured groups,
plain/quoted followups, own-post removal, retained text/manual recovery when a
group disappears, subscribed unread-group switching and the implicit-post checker.
Both mailers capture the article's subject/destination before editing and deliver
local replies only after the normal Cc confirmation. Cancellation keeps an older
draft, missing articles open no editor, and an invalid local destination retains
the new draft through the existing mailer behavior. Python independently checks
the retained draft bytes, two native message-save files and the mailbox receipt.

The existing NEWSREADER-only `send_news_reply` guard is unchanged. Board replies
use `begin_news_reply`, which starts the normal user mailer's editor and draft/Cc
flow. Normal room discovery, live player commands, elapsed scheduling, concurrent
players, remote mail and account save/restart are outside this fixture.
