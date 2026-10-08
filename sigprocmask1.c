/*
Example of signal sets and sigprocmask
Block and unblock signals
*/

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
int main() {
  sigset_t new_mask, old_mask;

  // Initialize the new signal mask.
  sigemptyset(&new_mask); // Before blocking - Clear
  sigaddset(&new_mask, SIGINT); // new_mask = {SIGINT}

  // blocks the signals in the new_mask set and returns the old signal mask in the old_mask variable.
  sigprocmask(SIG_SETMASK, &new_mask, &old_mask);// Why keep a copy in old_mask - to keep a copy of the current mask

  // Print a message.
  printf("SIGINT is blocked.\n");

  // Sleep for 8 seconds. Try Ctrl-C during this window: nothing happens.
  sleep(8);

  printf("Before restore\n");

  // Restore the old signal mask.
  sigprocmask(SIG_SETMASK, &old_mask, NULL); // Restore the old mask

  // Sleep for 8 seconds. A Ctrl-C from the blocked window arrives now.
  sleep(8);

  // Print a message.
  printf("All signals are unblocked.\n");

  // Sleep for 4 seconds.
  sleep(4);

  return 0;
}
