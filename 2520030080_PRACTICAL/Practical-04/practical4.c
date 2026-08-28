#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void create_children()
{
    pid_t children[3];

    printf("\n===== PART A: wait() vs waitpid() =====\n");

    for (int i = 0; i < 3; i++)
    {
        children[i] = fork();

        if (children[i] < 0)
        {
            perror("fork failed");
            exit(1);
        }

        if (children[i] == 0)
        {
            printf("Child %d: PID=%d, PPID=%d\n",
                   i + 1, getpid(), getppid());

            sleep((i + 1) * 2);

            printf("Child %d: terminating\n", i + 1);
            exit(10 + i);
        }
    }

    printf("\nParent PID=%d created 3 children.\n", getpid());

    /* wait() waits for any child */
    int status;
    pid_t finished = wait(&status);

    if (finished > 0)
    {
        printf("\nwait(): Child PID %d completed first.\n", finished);

        if (WIFEXITED(status))
        {
            printf("wait(): Exit status = %d\n",
                   WEXITSTATUS(status));
        }
    }

    /* waitpid() waits for a specific child */
    printf("\nwaitpid(): Waiting specifically for Child PID %d...\n",
           children[2]);

    if (waitpid(children[2], &status, 0) == children[2])
    {
        printf("waitpid(): Child PID %d completed.\n", children[2]);

        if (WIFEXITED(status))
        {
            printf("waitpid(): Exit status = %d\n",
                   WEXITSTATUS(status));
        }
    }

    /* Collect remaining children */
    while (wait(NULL) > 0)
    {
        printf("Parent collected remaining child.\n");
    }
}

void create_zombie()
{
    pid_t child = fork();

    if (child < 0)
    {
        perror("fork failed");
        exit(1);
    }

    if (child == 0)
    {
        printf("\n===== PART B: ZOMBIE PROCESS =====\n");
        printf("Zombie child PID: %d\n", getpid());
        printf("Child is exiting now...\n");

        exit(0);
    }
    else
    {
        printf("Parent PID: %d\n", getpid());
        printf("Child PID : %d\n", child);

        printf("\nParent will NOT call wait() immediately.\n");
        printf("The child becomes a zombie temporarily.\n");

        sleep(30);

        printf("\nParent now calls waitpid() to collect the child.\n");

        waitpid(child, NULL, 0);

        printf("Zombie process eliminated.\n");
    }
}

int main()
{
    printf("===== OSSP PRACTICAL-04 =====\n");
    printf("Process Synchronization and Zombie Processes\n");

    create_children();

    create_zombie();

    printf("\nProgram completed successfully.\n");

    return 0;
}
