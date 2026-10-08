/*
Example: Use sigaction() to establish a handler for the SIGINT signal.
*/

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
//  |sigaction| vs signal <<<<

// sigaction handles sets of signals 
// create mask of blocked signals
// functions to add signals to the mask
// functions to empty the mask
// Flags

static void handler(int signum)
{
    (void)signum;
    printf("Another action.\n");
}

int main()
{
    struct sigaction sa; // Struct with attributes: handler (pointer to a fct), sigaction (pointer to a fct), mask , flags.
    memset(&sa, 0, sizeof(sa));   // never leave sa_flags holding stack garbage

    // Define the action the signal will take
    sa.sa_handler = handler;

    // Empty the mask set for this sigaction -> this means that we can capture ALL signals
    sigemptyset(&sa.sa_mask);  // New

    // System calls interrupted by this signal are automatically restarted.
    //
    // SA_SIGINFO is deliberately NOT set here: it tells the kernel to call the
    // three-argument sa_sigaction member instead of sa_handler, so setting it
    // next to sa_handler asks for one thing and provides another. siginfo.c
    // shows the three-argument form done properly.
    sa.sa_flags = SA_RESTART;

    // Change the action of SIGINT
    if (sigaction(SIGINT, &sa, NULL) == -1)
        printf("sigaction failed!");

    for (;;)
        pause(); // The pause function suspends the calling process until a signal is received.
}
