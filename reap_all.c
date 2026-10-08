/*
Example: Reaping child processes
*/

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void do_something(int sig)
{
    (void)sig;
    printf("SIGCHLD received: change in status of child.\n");
}

int main()
{
    // signal handler: capture a change in the status of the child process and do somethind.
    signal(SIGCHLD, do_something);

    // Create five child processes
    // Each process sleeps for 5 seconds.
    for (int i = 1; i <= 5; i++)
    {
        // Fork a new child process
        int pid = fork();

        // If in the child process
        if (pid == 0)
        {
            sleep(5); // Sleep for five seconds. All die simultaneously
            printf("Child %d is exiting\n", i);
            return 0;
        }
    }

    // The parent process stays in an infinite loop
    // busy doing something else
    while (1)
    {
        printf("Parent is working \n");
        sleep(1);
    }
}
