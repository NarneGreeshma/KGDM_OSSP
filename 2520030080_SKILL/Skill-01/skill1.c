#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("===== OSSP SKILL-01: PROCESS MANAGEMENT =====\n");
    printf("Main Process PID  : %d\n", getpid());
    printf("Main Process PPID : %d\n\n", getppid());

    printf("Creating a child process using fork()...\n");

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process
        printf("\n--- CHILD PROCESS ---\n");
        printf("Child PID  : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());

        printf("\nChild is executing 'ls -l' using exec()...\n");

        execlp("ls", "ls", "-l", NULL);

        // This executes only if exec() fails
        perror("exec failed");
        exit(1);
    }

    else {
        // Parent process
        printf("\n--- PARENT PROCESS ---\n");
        printf("Parent PID : %d\n", getpid());
        printf("Child PID  : %d\n", pid);

        printf("\nParent is waiting for the child to finish...\n");

        wait(NULL);

        printf("Child process completed.\n");
        printf("Parent process completed.\n");
    }

    return 0;
}
