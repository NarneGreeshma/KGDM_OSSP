#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define INITIAL_BUFFER_SIZE 64

typedef struct HistoryNode
{
    char *command;
    struct HistoryNode *prev;
    struct HistoryNode *next;
} HistoryNode;

static struct termios original_terminal;

void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_terminal);
}

void enable_raw_mode(void)
{
    tcgetattr(STDIN_FILENO, &original_terminal);

    struct termios raw = original_terminal;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    atexit(disable_raw_mode);
}

void clear_line(size_t length)
{
    printf("\r\033[KOSSP> ");

    for (size_t i = 0; i < length; i++)
        putchar(' ');

    printf("\r\033[KOSSP> ");
    fflush(stdout);
}

HistoryNode *add_history(HistoryNode *tail, const char *command)
{
    HistoryNode *node = malloc(sizeof(HistoryNode));

    if (node == NULL)
    {
        perror("malloc");
        return tail;
    }

    node->command = malloc(strlen(command) + 1);

    if (node->command == NULL)
    {
        perror("malloc");
        free(node);
        return tail;
    }

    strcpy(node->command, command);

    node->prev = tail;
    node->next = NULL;

    if (tail != NULL)
        tail->next = node;

    return node;
}

void free_history(HistoryNode *head)
{
    while (head != NULL)
    {
        HistoryNode *next = head->next;

        free(head->command);
        free(head);

        head = next;
    }
}

char *read_input(HistoryNode *tail)
{
    size_t capacity = INITIAL_BUFFER_SIZE;
    size_t length = 0;

    char *buffer = malloc(capacity);

    if (buffer == NULL)
    {
        perror("malloc");
        return NULL;
    }

    buffer[0] = '\0';

    HistoryNode *cursor = tail;

    printf("OSSP> ");
    fflush(stdout);

    while (1)
    {
        unsigned char c;

        if (read(STDIN_FILENO, &c, 1) != 1)
            continue;

        /* ENTER */
        if (c == '\n' || c == '\r')
        {
            putchar('\n');
            buffer[length] = '\0';
            return buffer;
        }

        /* BACKSPACE */
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
         * Standard terminal UP/DOWN:
         *
         * UP   = ESC [ A
         * DOWN = ESC [ B
         *
         * Some environments may instead send UTF-8:
         *
         * UP   = E2 86 91
         * DOWN = E2 86 93
         */

        if (c == 27)
        {
            unsigned char a, b;

            if (read(STDIN_FILENO, &a, 1) != 1)
                continue;

            if (a != '[')
                continue;

            if (read(STDIN_FILENO, &b, 1) != 1)
                continue;

            if (b == 'A')
            {
                if (cursor != NULL)
                {
                    clear_line(length);

                    strcpy(buffer, cursor->command);
                    length = strlen(buffer);

                    printf("%s", buffer);
                    fflush(stdout);

                    cursor = cursor->prev;
                }

                continue;
            }

            if (b == 'B')
            {
                if (cursor != NULL && cursor->next != NULL)
                {
                    cursor = cursor->next;

                    clear_line(length);

                    strcpy(buffer, cursor->command);
                    length = strlen(buffer);

                    printf("%s", buffer);
                    fflush(stdout);
                }
                else
                {
                    clear_line(length);

                    buffer[0] = '\0';
                    length = 0;

                    cursor = NULL;
                }

                continue;
            }

            continue;
        }

        /* UTF-8 UP ARROW: E2 86 91 */
        if (c == 0xE2)
        {
            unsigned char a, b;

            if (read(STDIN_FILENO, &a, 1) != 1)
                continue;

            if (read(STDIN_FILENO, &b, 1) != 1)
                continue;

            if (a == 0x86 && b == 0x91)
            {
                if (cursor != NULL)
                {
                    clear_line(length);

                    strcpy(buffer, cursor->command);
                    length = strlen(buffer);

                    printf("%s", buffer);
                    fflush(stdout);

                    cursor = cursor->prev;
                }

                continue;
            }

            if (a == 0x86 && b == 0x93)
            {
                if (cursor != NULL && cursor->next != NULL)
                {
                    cursor = cursor->next;

                    clear_line(length);

                    strcpy(buffer, cursor->command);
                    length = strlen(buffer);

                    printf("%s", buffer);
                    fflush(stdout);
                }
                else
                {
                    clear_line(length);

                    buffer[0] = '\0';
                    length = 0;

                    cursor = NULL;
                }

                continue;
            }

            continue;
        }

        /* Ignore other control characters */
        if (c < 32)
            continue;

        /* Dynamically resize buffer */
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

        buffer[length++] = c;
        buffer[length] = '\0';

        putchar(c);
        fflush(stdout);
    }
}

int main(void)
{
    HistoryNode *head = NULL;
    HistoryNode *tail = NULL;

    printf("===== OSSP SKILL-03: COMMAND HISTORY =====\n");
    printf("Type commands and press Enter.\n");
    printf("Use UP/DOWN arrows to navigate history.\n");
    printf("Backspace is supported.\n");
    printf("Type 'exit' to quit.\n\n");

    enable_raw_mode();

    while (1)
    {
        char *input = read_input(tail);

        if (input == NULL)
            break;

        if (strlen(input) == 0)
        {
            free(input);
            continue;
        }

        if (strcmp(input, "exit") == 0)
        {
            free(input);
            break;
        }

        HistoryNode *new_tail = add_history(tail, input);

        if (head == NULL)
            head = new_tail;

        tail = new_tail;

        printf("Command stored: %s\n", input);

        free(input);
    }

    disable_raw_mode();
    free_history(head);

    printf("\nHistory memory released successfully.\n");
    printf("Program terminated.\n");

    return 0;
}
