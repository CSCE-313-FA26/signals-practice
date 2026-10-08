# CSCE 313 — signals practice

Twelve short C programs for the signals lecture: handlers, masks, timeouts, and
reaping children. They are the demos behind **Lab 4**, so anything you try here
transfers directly to `signals.cpp`.

Companion repositories: [`practice-1`](https://github.com/CSCE-313-FA26/practice-1)
(fork, wait, exec) and [`ipc-practice`](https://github.com/CSCE-313-FA26/ipc-practice)
(pipes, dup2, FIFOs).

```bash
git clone https://github.com/CSCE-313-FA26/signals-practice.git
cd signals-practice
make            # builds all twelve, no warnings
```

## The programs

Run times below were measured, not estimated. "Waits" means the program sits
there until you signal it — stop it with `Ctrl-C`, or `kill` it from a second
terminal.

| Program | What it shows | Runs for |
| --- | --- | --- |
| `sigint1` | `signal(SIGINT, SIG_IGN)` — Ctrl-C is discarded and the loop keeps going | 30 s |
| `sigint2` | the same job with `sigaction`, catching each Ctrl-C | 25 s, or Ctrl-C |
| `sigaction1` | the `struct sigaction` fields one at a time, then `pause()` | waits |
| `sigaction2` | `SA_RESETHAND` and `SA_NODEFER` against the default behaviour | instant |
| `sigaction3` | reading the **old** disposition back out of `sigaction()` | 10 s |
| `siginfo` | the three-argument handler: who sent the signal, and why | waits |
| `sigprocmask1` | blocking `SIGINT` with `SIG_SETMASK`, then restoring the old mask | 20 s |
| `sigprocmask2` | a *pending* signal: block, generate, inspect with `sigpending`, unblock | 10 s |
| `sigusr` | `SIGUSR1`/`SIGUSR2` sent from another terminal with `kill` | waits |
| `sleep` | `alarm()` + `pause()` — how `sleep()` is built | 5 s |
| `proc_reap` | a `SIGCHLD` handler that reaps: **0 zombies** | waits |
| `reap_all` | a `SIGCHLD` handler that forgets to reap: **5 zombies** | waits |

## Four demos worth doing live

**Ctrl-C, ignored and caught.** Run `./sigint1` and press Ctrl-C a few times:
nothing happens, because the disposition is `SIG_IGN`. Then run `./sigint2` and
press it again — the handler prints, and `sleep()` returns early:

```
Sleeping...
^C
Signal caught!
Awake
```

**A pending signal.** `./sigprocmask2` blocks `SIGQUIT`, sleeps five seconds,
then asks what is pending. Press `Ctrl-\` three times during the sleep:

```
^\^\^\
SIGQUIT pending
caught SIGQUIT
SIGQUIT unblocked
```

Three presses, **one** delivery. Ordinary signals do not queue: a signal is
either pending or not. Press `Ctrl-\` once more after "unblocked" and the
default action takes over — quit, with a core dump.

**Zombies, with and without reaping.** Start `./reap_all`, wait five seconds,
then in another terminal:

```bash
ps --ppid $(pgrep -x reap_all) -o pid,stat,comm
```

Five children, every one in state `Z`. The handler prints but never calls
`wait`, so the kernel keeps each exit status for a parent that never collects
it. `./proc_reap` is the same program with `wait(0)` in the handler, and ends
with no children and no zombies.

**Who sent the signal.** `./siginfo` installs a three-argument handler with
`SA_SIGINFO`, forks a child, and the child `exec`s `hello.sh`, which exits 7:

```
Child process: PID = 1078328
Welcome to CSCE 313
signal 17 received with siginfo_t:
	signal number: si_signo=17
	signal code: si_code=1
	sending process ID: si_pid=1078328
	exit value or signal: si_status=7
```

Signal 17 is `SIGCHLD`; `si_code=1` is `CLD_EXITED`; `si_status=7` is the
script's own exit status. Only the fields that apply to this signal are
meaningful — `si_band` and `si_timerid` print whatever happens to be in the
union.

## Signal sets: what the three `sigprocmask` commands do

`sigprocmask(how, &new, &old)` changes the process's blocked-signal mask. The
three values of `how` are set operations, and mixing them up is the usual
source of confusion:

| `how` | current mask | new set | result |
| --- | --- | --- | --- |
| `SIG_BLOCK` | {A, B} | {C} | {A, B, C} — union |
| `SIG_UNBLOCK` | {A, B} | {B, C} | {A} — current minus new |
| `SIG_SETMASK` | {A, B} | {B, C} | {B, C} — replaced outright |

```c
sigset_t new_set;
sigemptyset(&new_set);
sigaddset(&new_set, SIGINT);
sigaddset(&new_set, SIGTERM);
sigprocmask(SIG_BLOCK, &new_set, NULL);     /* block both, keep what was blocked */
```

Note that `sigprocmask1.c` uses `SIG_SETMASK`, so it **replaces** the mask
rather than adding to it. That is why it saves the old mask and restores it
afterwards.

## Two things these examples get wrong on purpose

**`printf` in a signal handler is not safe.** Almost every handler here calls
`printf`, because it keeps the examples readable. A handler may only call
async-signal-safe functions, and `printf` is not one: it takes a lock and
touches a buffer that the interrupted code may have been halfway through
updating. It usually appears to work, which is what makes it dangerous. Lab 4
requires `write()` instead — see section 2.3 of the Lab 4 handout.

**`reap_all` does not reap.** The name describes the intention, not the code.
Keep it: the zombie table it produces is the point.

## `signal()` versus `sigaction()`, measured

`signal()`'s behaviour depends on which feature macros the file was compiled
with. A small probe, compiled two ways, asking for the disposition right after
the handler ran:

| Built with | After one delivery | Second signal |
| --- | --- | --- |
| `-std=c11 -D_POSIX_C_SOURCE=200809L` | `SIG_DFL` | kills the process (exit 138) |
| `-std=gnu11` | still our handler | caught again |

Under strict POSIX, glibc gives `signal()` its System V semantics: the handler
is uninstalled as soon as it runs. Every example here assumes the handler stays,
so the `Makefile` uses `-std=gnu11` — which is also what plain `gcc file.c`
gives you.

`sigaction()` has no such ambiguity. You state the flags you want, and
`SA_RESETHAND` is there if you actually want the one-shot behaviour — which is
what `sigaction2.c` demonstrates. This is the reason Lab 4 requires `sigaction`.

## Output that disappears

Piped or redirected, `stdout` is block-buffered, so a program that is killed
before it flushes prints nothing at all:

```bash
./sigusr > out.txt      # kill it, and out.txt can be empty
./sigusr                # on a terminal, line-buffered, prints immediately
```

That is not a bug in the program. If you need output from a program you are
going to kill, run it on a terminal, or call `fflush(stdout)`, or write with
`write()`.

## Differences from the lecture copies

| Change | Why |
| --- | --- |
| `#include "apue.h"` replaced with the real headers | the examples used no APUE helper functions, only the umbrella header, so the book's header was dropped and each file is now self-contained |
| `sigint2.c`: `struct sigaction` is `memset` to zero and `sa_flags` set to 0 | it was uninitialised, so `sigaction()` acted on whatever flags happened to be on the stack |
| `sigaction1.c`: `SA_SIGINFO` removed from `sa_flags` | it was set next to `sa_handler`; `SA_SIGINFO` tells the kernel to call the three-argument `sa_sigaction` instead. `siginfo.c` shows that form done properly |
| `sigint1.c` loops 6 times instead of 500 | 500 iterations of `sleep(5)` is 42 minutes |
| `sigprocmask1.c` 20/20/10 s → 8/8/4; `sigaction3.c` 20 s → 10 s | classroom-sized |
| `(void)parameter;` added where a handler ignores its argument | so the whole set builds clean under `-Wall -Wextra` |
| Typographic quotes and `ˆ` replaced with ASCII | they were inside C string literals and comments |
| 13 stale binaries, `apue.h`, `info.sh` and an unused `files/` tree dropped | nothing builds or reads them; `hello.sh` is kept because `siginfo.c` execs it |
| `examples.c` is not in the repository | it was lecture notes, not a program: bare statements outside any function, which cannot compile. Its content is the signal-set table above |
