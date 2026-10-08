/*
Example: Signal handler for CTRL-C using sigaction
*/

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
void sighandler(int signum)
{
    (void)signum;
    printf("\nSignal caught!\n");
    ///....... Additional incoming SIGINTs are blocked

    /// Else

    // Unblock the same signal (SIGINT) -->
}

int main()
{
    /* Zero the whole struct first. Without this, sa_flags holds whatever was
       on the stack, and sigaction() would act on flags you never chose. */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = sighandler;
    sa.sa_flags = 0;
    sigemptyset(&(sa.sa_mask));
    sigaddset(&(sa.sa_mask), SIGINT);
    sigaction(SIGINT, &sa, NULL);

    for (int i = 0; i < 5; i++)
    {
        printf("Sleeping...\n");
        sleep(5);
        printf("Awake\n");
    }
}