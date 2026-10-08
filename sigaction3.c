#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void custom_sigint_handler(int signum) {
    printf("Custom handler called for signal %d (SIGINT)\n", signum);
}

int main() {
    // Create a sigaction structure to store the old signal handling behavior
    struct sigaction old_action;
    old_action.sa_flags = SA_NODEFER;
    sigemptyset(&old_action.sa_mask);

    // Set up a custom signal handler for SIGINT
    struct sigaction new_action;
    new_action.sa_handler = custom_sigint_handler;
    new_action.sa_flags = 0;
    sigemptyset(&new_action.sa_mask);

    // Get the current signal handling behavior for SIGINT
    if (sigaction(SIGINT, &new_action, &old_action) == 0) {
        printf("Custom signal handler for SIGINT has been set.\n");

        // To access the old behavior, you can examine the old_action structure
        printf("New handler address: %p\n", (void *)(new_action.sa_handler));
        //printf("New handler address: %p\n", (void *)(old_action.sa_handler));
    } else {
        perror("sigaction");
        return 1;
    }

    // Trigger a SIGINT signal
    //raise(SIGINT);
    sleep(10);   // press Ctrl-C during this window

    // Continue program execution

    return 0;
}
