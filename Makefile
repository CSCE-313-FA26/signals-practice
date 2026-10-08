# CSCE 313 - signals practice
#
# -std=gnu11, not -std=c11, and the reason is not cosmetic. Under a strict
# -std=c11 with -D_POSIX_C_SOURCE, glibc gives signal() its System V semantics:
# the disposition resets to SIG_DFL as soon as the handler runs, so the SECOND
# signal takes the default action and kills the process. Measured with a probe:
#
#     -std=c11 -D_POSIX_C_SOURCE=200809L   after delivery: SIG_DFL, 2nd signal kills
#     -std=gnu11                           after delivery: our handler, 2nd signal caught
#
# Every example here that uses signal() assumes the handler stays installed, so
# gnu11 is the honest default. It is also what plain `gcc file.c` gives you.
# This is one more argument for sigaction(), which states its behaviour instead
# of inheriting it from a feature macro.
CC      = gcc
CFLAGS  = -std=gnu11 -Wall -Wextra -g

PROGS = sigint1 sigint2 sigaction1 sigaction2 sigaction3 siginfo \
        sigprocmask1 sigprocmask2 sigusr sleep proc_reap reap_all

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(PROGS)

.PHONY: all clean
