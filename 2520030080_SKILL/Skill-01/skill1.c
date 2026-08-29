#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define INITIAL_BUFFER_SIZE 64

/*
 * Read a command from the user using a dynamically
 * allocated input buffer.
 */
char *read_command(void)
{
    size_t capacity = INITIAL_BUFFER_SIZE;
    size_t length = 0;

    char *buffer = malloc(capacity);

    if (buffer == NULL)
    {
        perror("malloc");
        return NULL;
    }

    while (1)
    {
        int c = getchar();

        /* EOF */
        if (c == EOF)
        {
            free(buffer);
            return NULL;
        }

        /* Enter key */
        if (c == '\n')
        {
            buffer[length] = '\0';
            printf("\n");
            break;
        }

        /* Backspace */
        if (c == 127 || c == '\b')
        {
            if (length > 0)
            {
                length--;
                buffer[length] = '\0';

                printf("\b \b");
                fflush(stdout);
            }

            continue;
        }

        /*
         * Dynamically resize the input buffer
         * when it becomes full.
         */
        if (length + 1 >= capacity)
        {
            size_t new_capacity = capacity * 2;

            char *new_buffer =
                realloc(buffer, new_capacity);

            if (new_buffer == NULL)
            {
                perror("realloc");
                free(buffer);
                return NULL;
            }

            buffer = new_buffer;
            capacity = new_capacity;
        }

        buffer[length++] = (char)c;
        buffer[length] = '\0';

        putchar(c);
        fflush(stdout);
    }

    return buffer;
}


/*
 * Split the input command into arguments.
 * Whitespace is treated as the delimiter.
 */
char **parse_command(char *command, int *argc)
{
    size_t capacity = 8;
    int count = 0;

    char **argv = malloc(capacity * sizeof(char *));

    if (argv == NULL)
    {
        perror("malloc");
        return NULL;
    }

    char *token = strtok(command, " \t");

    while (token != NULL)
    {
        if ((size_t)(count + 1) >= capacity)
        {
            capacity *= 2;

            char **new_argv =
                realloc(argv,
                        capacity * sizeof(char *));

            if (new_argv == NULL)
            {
                perror("realloc");
                free(argv);
                return NULL;
            }

            argv = new_argv;
        }

        argv[count++] = token;

        token = strtok(NULL, " \t");
    }

    argv[count] = NULL;
    *argc = count;

    return argv;
}


/*
 * Execute a command using fork() and execvp().
 */
int execute_command(char **argv)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        /*
         * Child process
         */
        printf("\n[Child]\n");
        printf("PID  : %d\n", getpid());
        printf("PPID : %d\n", getppid());

        printf("Executing command using execvp()...\n\n");

        execvp(argv[0], argv);

        /*
         * Reached only if execvp() fails.
         */
        fprintf(stderr,
                "execvp failed for '%s': %s\n",
                argv[0],
                strerror(errno));

        exit(EXIT_FAILURE);
    }

    /*
     * Parent process
     */
    printf("[Parent]\n");
    printf("PID       : %d\n", getpid());
    printf("Child PID : %d\n", pid);

    printf("Parent waiting for child %d...\n", pid);

    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status))
    {
        printf("\nChild %d exited with status %d.\n",
               pid,
               WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("\nChild %d terminated by signal %d.\n",
               pid,
               WTERMSIG(status));
    }

    return 0;
}


/*
 * Main interactive shell loop.
 */
int main(void)
{
    printf("============================================\n");
    printf("       OSSP SKILL-01 MINI SHELL\n");
    printf("============================================\n");

    printf("Shell PID  : %d\n", getpid());
    printf("Shell PPID : %d\n\n", getppid());

    printf("Supported features:\n");
    printf("- Interactive command loop\n");
    printf("- fork() and execvp()\n");
    printf("- Parent-child process management\n");
    printf("- Dynamic input buffer\n");
    printf("- Backspace handling\n");
    printf("- Multi-character commands\n");
    printf("- Type 'exit' to terminate\n\n");

    while (1)
    {
        printf("OSSP> ");
        fflush(stdout);

        char *command = read_command();

        if (command == NULL)
        {
            printf("\nEOF received. Exiting shell.\n");
            break;
        }

        /*
         * Empty command
         */
        if (strlen(command) == 0)
        {
            free(command);
            continue;
        }

        /*
         * Exit condition
         */
        if (strcmp(command, "exit") == 0)
        {
            free(command);
            printf("Exiting OSSP shell...\n");
            break;
        }

        int argc;

        char **argv =
            parse_command(command, &argc);

        if (argv == NULL)
        {
            free(command);
            continue;
        }

        if (argc == 0)
        {
            free(argv);
            free(command);
            continue;
        }

        /*
         * Execute command.
         */
        execute_command(argv);

        free(argv);
        free(command);

        printf("\n");
    }

    printf("Shell terminated successfully.\n");

    return 0;
}
