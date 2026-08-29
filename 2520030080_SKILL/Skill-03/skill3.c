#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

/*
 * SKILL-03
 * --------
 * 1. Single quotes preserve literal content and prevent expansion.
 * 2. Double quotes preserve spaces and allow variable expansion.
 * 3. Backslash escapes the next character.
 * 4. Parsed commands are executed using fork() and execvp().
 */

typedef struct
{
    char *text;
    int single_quoted;
    int double_quoted;
} Token;

/* Expand $NAME and ${NAME} inside double-quoted/unquoted text. */
char *expand_variables(const char *input)
{
    size_t capacity = 64;
    size_t length = 0;

    char *result = malloc(capacity);

    if (result == NULL)
    {
        perror("malloc");
        return NULL;
    }

    for (size_t i = 0; input[i] != '\0';)
    {
        if (input[i] != '$')
        {
            if (length + 2 >= capacity)
            {
                capacity *= 2;
                char *tmp = realloc(result, capacity);

                if (tmp == NULL)
                {
                    perror("realloc");
                    free(result);
                    return NULL;
                }

                result = tmp;
            }

            result[length++] = input[i++];
            continue;
        }

        i++;

        if (input[i] == '{')
        {
            i++;

            size_t start = i;

            while (input[i] != '\0' &&
                   (input[i] == '_' ||
                    (input[i] >= 'a' && input[i] <= 'z') ||
                    (input[i] >= 'A' && input[i] <= 'Z') ||
                    (input[i] >= '0' && input[i] <= '9'))
            )
            {
                i++;
            }

            size_t name_length = i - start;

            if (input[i] == '}')
                i++;

            char name[256];

            if (name_length >= sizeof(name))
                name_length = sizeof(name) - 1;

            memcpy(name, input + start, name_length);
            name[name_length] = '\0';

            const char *value = getenv(name);

            if (value == NULL)
                value = "";

            size_t value_length = strlen(value);

            while (length + value_length + 1 >= capacity)
                capacity *= 2;

            char *tmp = realloc(result, capacity);

            if (tmp == NULL)
            {
                perror("realloc");
                free(result);
                return NULL;
            }

            result = tmp;

            memcpy(result + length, value, value_length);
            length += value_length;
        }
        else
        {
            size_t start = i;

            while (input[i] != '\0' &&
                   (input[i] == '_' ||
                    (input[i] >= 'a' && input[i] <= 'z') ||
                    (input[i] >= 'A' && input[i] <= 'Z') ||
                    (input[i] >= '0' && input[i] <= '9'))
            )
            {
                i++;
            }

            if (start == i)
            {
                if (length + 2 >= capacity)
                {
                    capacity *= 2;
                    char *tmp = realloc(result, capacity);

                    if (tmp == NULL)
                    {
                        perror("realloc");
                        free(result);
                        return NULL;
                    }

                    result = tmp;
                }

                result[length++] = '$';
            }
            else
            {
                size_t name_length = i - start;

                char name[256];

                if (name_length >= sizeof(name))
                    name_length = sizeof(name) - 1;

                memcpy(name, input + start, name_length);
                name[name_length] = '\0';

                const char *value = getenv(name);

                if (value == NULL)
                    value = "";

                size_t value_length = strlen(value);

                while (length + value_length + 1 >= capacity)
                    capacity *= 2;

                char *tmp = realloc(result, capacity);

                if (tmp == NULL)
                {
                    perror("realloc");
                    free(result);
                    return NULL;
                }

                result = tmp;

                memcpy(result + length, value, value_length);
                length += value_length;
            }
        }
    }

    result[length] = '\0';
    return result;
}

/* Add one character to a dynamically allocated token. */
int append_char(char **buffer, size_t *length, size_t *capacity, char c)
{
    if (*length + 2 >= *capacity)
    {
        *capacity *= 2;

        char *tmp = realloc(*buffer, *capacity);

        if (tmp == NULL)
        {
            perror("realloc");
            return 0;
        }

        *buffer = tmp;
    }

    (*buffer)[(*length)++] = c;
    (*buffer)[*length] = '\0';

    return 1;
}

/*
 * Tokenize shell input.
 *
 * Single quotes:
 *   'hello $USER'
 * are kept literal.
 *
 * Double quotes:
 *   "hello $USER"
 * preserve spaces and expand variables.
 *
 * Backslash:
 *   hello\ world
 * produces one token: "hello world".
 */
int tokenize(const char *input, Token tokens[], int *count)
{
    size_t i = 0;
    *count = 0;

    while (input[i] != '\0')
    {
        while (input[i] == ' ' || input[i] == '\t')
            i++;

        if (input[i] == '\0')
            break;

        if (*count >= MAX_ARGS - 1)
        {
            fprintf(stderr, "Error: too many tokens.\n");
            return 0;
        }

        size_t capacity = 64;
        size_t length = 0;

        char *buffer = malloc(capacity);

        if (buffer == NULL)
        {
            perror("malloc");
            return 0;
        }

        buffer[0] = '\0';

        int single_quoted = 0;
        int double_quoted = 0;

        while (input[i] != '\0')
        {
            char c = input[i];

            if (!single_quoted && !double_quoted &&
                (c == ' ' || c == '\t'))
            {
                break;
            }

            /* Single quote */
            if (!double_quoted && c == '\'')
            {
                single_quoted = !single_quoted;
                i++;
                continue;
            }

            /* Double quote */
            if (!single_quoted && c == '"')
            {
                double_quoted = !double_quoted;
                i++;
                continue;
            }

            /* Escape next character */
            if (!single_quoted && c == '\\')
            {
                i++;

                if (input[i] == '\0')
                {
                    fprintf(stderr,
                            "Syntax Error: trailing escape character.\n");
                    free(buffer);
                    return 0;
                }

                if (!append_char(&buffer, &length, &capacity, input[i]))
                {
                    free(buffer);
                    return 0;
                }

                i++;
                continue;
            }

            /*
             * Variable expansion is disabled inside
             * single quotes.
             */
            if (!single_quoted && c == '$')
            {
                size_t start = i;

                i++;

                if (input[i] == '{')
                {
                    i++;

                    while (input[i] != '\0' &&
                           input[i] != '}')
                    {
                        i++;
                    }

                    if (input[i] == '}')
                        i++;
                }
                else
                {
                    while (input[i] != '\0' &&
                           (input[i] == '_' ||
                            (input[i] >= 'a' && input[i] <= 'z') ||
                            (input[i] >= 'A' && input[i] <= 'Z') ||
                            (input[i] >= '0' && input[i] <= '9')))
                    {
                        i++;
                    }
                }

                size_t expression_length = i - start;

                char *expression = malloc(expression_length + 1);

                if (expression == NULL)
                {
                    perror("malloc");
                    free(buffer);
                    return 0;
                }

                memcpy(expression,
                       input + start,
                       expression_length);

                expression[expression_length] = '\0';

                char *expanded = expand_variables(expression);

                free(expression);

                if (expanded == NULL)
                {
                    free(buffer);
                    return 0;
                }

                for (size_t j = 0; expanded[j] != '\0'; j++)
                {
                    if (!append_char(&buffer,
                                     &length,
                                     &capacity,
                                     expanded[j]))
                    {
                        free(expanded);
                        free(buffer);
                        return 0;
                    }
                }

                free(expanded);
                continue;
            }

            if (!append_char(&buffer, &length, &capacity, c))
            {
                free(buffer);
                return 0;
            }

            i++;
        }

        if (single_quoted)
        {
            fprintf(stderr,
                    "Syntax Error: unmatched single quote.\n");
            free(buffer);
            return 0;
        }

        if (double_quoted)
        {
            fprintf(stderr,
                    "Syntax Error: unmatched double quote.\n");
            free(buffer);
            return 0;
        }

        tokens[*count].text = buffer;
        tokens[*count].single_quoted = single_quoted;
        tokens[*count].double_quoted = double_quoted;

        (*count)++;

        while (input[i] == ' ' || input[i] == '\t')
            i++;
    }

    tokens[*count].text = NULL;

    return 1;
}

void free_tokens(Token tokens[], int count)
{
    for (int i = 0; i < count; i++)
        free(tokens[i].text);
}

/* Display the parser result for debugging/validation. */
void print_tokens(Token tokens[], int count)
{
    printf("\n--- TOKEN STREAM ---\n");

    if (count == 0)
    {
        printf("(empty)\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        printf("[%d] \"%s\"", i, tokens[i].text);

        if (tokens[i].single_quoted)
            printf(" [single-quoted]");

        if (tokens[i].double_quoted)
            printf(" [double-quoted]");

        printf("\n");
    }

    printf("Token stream is valid.\n");
}

/* Execute a parsed command using fork() and execvp(). */
void execute_command(Token tokens[], int count)
{
    if (count == 0)
        return;

    char *argv[MAX_ARGS];

    for (int i = 0; i < count; i++)
        argv[i] = tokens[i].text;

    argv[count] = NULL;

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        printf("\n[Child]\n");
        printf("PID : %d\n", getpid());
        printf("Executing: %s\n\n", argv[0]);

        execvp(argv[0], argv);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    printf("\n[Parent]\n");
    printf("PID       : %d\n", getpid());
    printf("Child PID : %d\n", pid);

    int status;

    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        return;
    }

    if (WIFEXITED(status))
    {
        printf("Child exited with status %d.\n",
               WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("Child terminated by signal %d.\n",
               WTERMSIG(status));
    }
}

int main(void)
{
    char input[MAX_INPUT];

    printf("============================================\n");
    printf("       OSSP SKILL-03 QUOTING & EXECUTION\n");
    printf("============================================\n");

    printf("Features:\n");
    printf("- Single quotes preserve literal content\n");
    printf("- Double quotes preserve spaces and expand variables\n");
    printf("- Backslash escape sequences\n");
    printf("- fork() and execvp()\n");
    printf("- Parent-child process management\n");
    printf("- Syntax validation\n");
    printf("- Type 'exit' to terminate\n\n");

    while (1)
    {
        printf("OSSP> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting OSSP shell...\n");
            break;
        }

        if (input[0] == '\0')
        {
            printf("Empty command.\n\n");
            continue;
        }

        Token tokens[MAX_ARGS];

        int count = 0;

        if (!tokenize(input, tokens, &count))
        {
            printf("\n");
            continue;
        }

        print_tokens(tokens, count);

        if (count > 0)
            execute_command(tokens, count);

        free_tokens(tokens, count);

        printf("\n");
    }

    printf("Shell terminated successfully.\n");

    return 0;
}
