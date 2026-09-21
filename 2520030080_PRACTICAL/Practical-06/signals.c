#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig)
{
    if (sig == SIGINT)
        printf("\nSIGINT received\n");
    else if (sig == SIGTERM)
        printf("SIGTERM received\n");
    else if (sig == SIGUSR1)
        printf("SIGUSR1 received\n");
}

int main()
{
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGUSR1, handler);

    printf("PID: %d\n", getpid());

    while (1)
    {
        printf("Waiting for signal...\n");
        sleep(3);
    }

    return 0;
}
