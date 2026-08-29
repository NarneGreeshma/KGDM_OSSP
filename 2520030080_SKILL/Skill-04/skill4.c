#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 20

/* ---------- Variable Expansion ---------- */

void expand_variables(char *input, char *output, size_t size)
{
    char variable[100];
    const char *value;
    size_t i = 0, j = 0;

    while (input[i] != '\0' && j < size - 1)
    {
        if (input[i] == '$')
        {
            i++;
            int k = 0;

            while ((input[i] >= 'A' && input[i] <= 'Z') ||
                   (input[i] >= 'a' && input[i] <= 'z') ||
                   (input[i] >= '0' && input[i] <= '9') ||
                   input[i] == '_')
            {
                if (k < 99)
                    variable[k++] = input[i];

                i++;
            }

            variable[k] = '\0';

            value = getenv(variable);

            if (value == NULL)
                value = "";

            while (*value && j < size - 1)
                output[j++] = *value++;
        }
        else
        {
            output[j++] = input[i++];
        }
    }

    output[j] = '\0';
}

/* ---------- PATH Resolution ---------- */

char *find_executable(char *command)
{
    static char fullpath[512];

    char *path = getenv("PATH");

    if (path == NULL)
        return NULL;

    char *copy = malloc(strlen(path) + 1);

    if (copy == NULL)
        return NULL;

    strcpy(copy, path);

    char *dir = strtok(copy, ":");

    while (dir != NULL)
    {
        snprintf(fullpath,
                 sizeof(fullpath),
                 "%s/%s",
                 dir,
                 command);

        if (access(fullpath, X_OK) == 0)
        {
            free(copy);
            return fullpath;
        }

        dir = strtok(NULL, ":");
    }

    free(copy);
    return NULL;
}

/* ---------- Built-ins ---------- */

int builtin_cd(char **args)
{
    if (args[1] == NULL)
    {
        printf("cd: missing directory\n");
        return 1;
    }

    if (chdir(args[1]) != 0)
    {
        perror("cd");
        return 1;
    }

    return 0;
}

int builtin_pwd(char **args)
{
    (void)args;

    char cwd[512];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("pwd");

    return 0;
}

int builtin_help(char **args)
{
    (void)args;

    printf("Built-ins: cd, pwd, help, exit\n");

    return 0;
}

typedef int (*BuiltinFunction)(char **);

typedef struct
{
    char *name;
    BuiltinFunction function;
} Builtin;

Builtin builtins[] =
{
    {"cd", builtin_cd},
    {"pwd", builtin_pwd},
    {"help", builtin_help},
    {NULL, NULL}
};

BuiltinFunction find_builtin(char *command)
{
    for (int i = 0; builtins[i].name != NULL; i++)
    {
        if (strcmp(command, builtins[i].name) == 0)
            return builtins[i].function;
    }

    return NULL;
}

/* ---------- Execute External Command ---------- */

void execute_command(char **args)
{
    char *path = find_executable(args[0]);

    if (path == NULL)
    {
        printf("%s: command not found\n", args[0]);
        return;
    }

    printf("[PATH Resolution]\n");
    printf("Command    : %s\n", args[0]);
    printf("Executable : %s\n", path);

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        /* Child */
        execv(path, args);

        perror("execv");
        exit(1);
    }

    /* Parent */
    int status;

    printf("[Parent] Waiting for child %d using waitpid()...\n",
           pid);

    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
    {
        printf("[Parent] Child exited with status %d\n",
               WEXITSTATUS(status));
    }
}

/* ---------- Main ---------- */

int main(void)
{
    char input[512];
    char expanded[512];
    char *args[MAX_ARGS];

    printf("========================================\n");
    printf("      OSSP SKILL-04: SHELL EXTENSIONS\n");
    printf("========================================\n");

    printf("Features:\n");
    printf("- waitpid() synchronization\n");
    printf("- PATH resolution\n");
    printf("- Variable expansion\n");
    printf("- Built-in dispatch table\n");
    printf("- Command-not-found handling\n\n");

    while (1)
    {
        printf("OSSP> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        /* Empty command */
        if (strlen(input) == 0)
        {
            printf("Empty command.\n");
            continue;
        }

        /* Variable expansion */
        expand_variables(input,
                         expanded,
                         sizeof(expanded));

        printf("[Expanded] %s\n", expanded);

        /* Tokenize */
        int argc = 0;

        char *token = strtok(expanded, " \t");

        while (token != NULL &&
               argc < MAX_ARGS - 1)
        {
            args[argc++] = token;
            token = strtok(NULL, " \t");
        }

        args[argc] = NULL;

        if (argc == 0)
            continue;

        /* Exit */
        if (strcmp(args[0], "exit") == 0)
        {
            printf("Shell exiting...\n");
            break;
        }

        /* Built-in dispatch */
        BuiltinFunction builtin =
            find_builtin(args[0]);

        if (builtin != NULL)
        {
            printf("[Built-in] Executing '%s' in parent process\n",
                   args[0]);

            builtin(args);
        }
        else
        {
            execute_command(args);
        }

        printf("\n");
    }

    printf("Program terminated successfully.\n");

    return 0;
}
