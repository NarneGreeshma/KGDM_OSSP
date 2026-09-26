#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main()
{
    char input[200], *args[20];
    int i, n;

    while (1)
    {
        printf("OSSP> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (!strcmp(input, "exit"))
            break;

        n = 0;
        char *p = strtok(input, " ");

        while (p && n < 19)
        {
            args[n++] = p;
            p = strtok(NULL, " ");
        }

        args[n] = NULL;

        if (!args[0])
            continue;

        pid_t pid = fork();

        if (pid == 0)
        {
            int fd;

            for (i = 0; i < n; i++)
            {
                if (!strcmp(args[i], "<") && args[i + 1])
                {
                    fd = open(args[i + 1], O_RDONLY);
                    if (fd < 0)
                    {
                        perror("Input");
                        exit(1);
                    }

                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    args[i] = NULL;
                }

                else if (!strcmp(args[i], ">") && args[i + 1])
                {
                    fd = open(args[i + 1],
                              O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0)
                    {
                        perror("Output");
                        exit(1);
                    }

                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    args[i] = NULL;
                }

                else if (!strcmp(args[i], ">>") && args[i + 1])
                {
                    fd = open(args[i + 1],
                              O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (fd < 0)
                    {
                        perror("Append");
                        exit(1);
                    }

                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    args[i] = NULL;
                }

                else if (!strcmp(args[i], "2>") && args[i + 1])
                {
                    fd = open(args[i + 1],
                              O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0)
                    {
                        perror("Error");
                        exit(1);
                    }

                    dup2(fd, STDERR_FILENO);
                    close(fd);
                    args[i] = NULL;
                }
            }

            execvp(args[0], args);
            perror("Command");
            exit(1);
        }

        waitpid(pid, NULL, 0);
    }

    return 0;
}
