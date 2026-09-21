#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXH 5
#define MAXC 10

char history[MAXH][200];
int hcount = 0;

void save_history(char *s)
{
    if (hcount < MAXH)
        strcpy(history[hcount++], s);
    else
    {
        for (int i = 1; i < MAXH; i++)
            strcpy(history[i - 1], history[i]);
        strcpy(history[MAXH - 1], s);
    }
}

void show_history()
{
    for (int i = 0; i < hcount; i++)
        printf("%d %s\n", i + 1, history[i]);
}

void run_pipeline(char *input)
{
    char *cmd[MAXC], *save;
    int n = 0, fd[2], in = 0;
    pid_t pid[MAXC];

    cmd[n++] = strtok_r(input, "|", &save);

    while ((cmd[n] = strtok_r(NULL, "|", &save)) != NULL)
        n++;

    if (n > MAXC)
    {
        printf("Too many commands\n");
        return;
    }

    for (int i = 0; i < n; i++)
    {
        char *args[10], *p, *s;
        int j = 0;

        p = strtok_r(cmd[i], " \t", &s);
        while (p && j < 9)
        {
            args[j++] = p;
            p = strtok_r(NULL, " \t", &s);
        }
        args[j] = NULL;

        if (!args[0])
            return;

        if (i < n - 1)
            pipe(fd);

        pid[i] = fork();

        if (pid[i] == 0)
        {
            if (in != 0)
            {
                dup2(in, STDIN_FILENO);
                close(in);
            }

            if (i < n - 1)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            }

            execvp(args[0], args);
            perror("Command");
            return;
        }

        if (in != 0)
            close(in);

        if (i < n - 1)
        {
            close(fd[1]);
            in = fd[0];
        }
    }

    for (int i = 0; i < n; i++)
        waitpid(pid[i], NULL, 0);
}

int main()
{
    char input[200];

    while (1)
    {
        printf("OSSP> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (!strcmp(input, "exit"))
            break;

        save_history(input);

        if (!strcmp(input, "history"))
            show_history();
        else
            run_pipeline(input);
    }

    return 0;
}
