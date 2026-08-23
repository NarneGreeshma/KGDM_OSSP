#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void show_status(const char *stage)
{
    char path[100];
    char line[256];
    FILE *fp;

    snprintf(path, sizeof(path), "/proc/%d/status", getpid());

    fp = fopen(path, "r");

    printf("\n========== %s ==========\n", stage);
    printf("PID  : %d\n", getpid());
    printf("PPID : %d\n", getppid());

    if (fp != NULL)
    {
        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "State:", 6) == 0)
            {
                printf("%s", line);
                break;
            }
        }

        fclose(fp);
    }
}

int main()
{
    pid_t pid;

    printf("===== OSSP PRACTICAL-03: PROCESS MANAGEMENT =====\n");

    show_status("PARENT BEFORE fork()");

    pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }

    if (pid == 0)
    {
        /* Child process */
        show_status("CHILD - RUNNING");

        printf("\nChild is sleeping for 40 seconds...\n");
        printf("Use another terminal to inspect this process using ps/top/proc.\n");

        sleep(40);

        show_status("CHILD - RUNNING AFTER WAIT");

        printf("\nChild process terminating...\n");
        exit(0);
    }
    else
    {
        /* Parent process */
        show_status("PARENT - RUNNING");

        printf("\nParent PID : %d\n", getpid());
        printf("Child PID  : %d\n", pid);

        printf("\nParent is waiting for child to terminate...\n");

        wait(NULL);

        show_status("PARENT - AFTER wait()");

        printf("\nChild has terminated.\n");
        printf("Parent process terminating...\n");
    }

    return 0;
}
