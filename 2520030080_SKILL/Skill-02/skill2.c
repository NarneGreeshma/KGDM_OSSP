#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define BUFFER_SIZE 100

void enableRawMode(struct termios *original)
{
    struct termios raw;

    tcgetattr(STDIN_FILENO, original);

    raw = *original;

    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode(struct termios *original)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, original);
}

int main()
{
    char input[BUFFER_SIZE];
    int position;
    char ch;

    struct termios original;

    enableRawMode(&original);

    printf("===== OSSP SKILL-02: INTERACTIVE INPUT =====\n");
    printf("Type a command and press Enter.\n");
    printf("Type 'exit' to quit.\n\n");

    while (1)
    {
        position = 0;
        memset(input, 0, BUFFER_SIZE);

        printf("OSSP> ");
        fflush(stdout);

        while (1)
        {
            read(STDIN_FILENO, &ch, 1);

            /* Handle Enter key */
            if (ch == '\n' || ch == '\r')
            {
                input[position] = '\0';
                printf("\n");
                break;
            }

            /* Handle Backspace */
            if (ch == 127 || ch == '\b')
            {
                if (position > 0)
                {
                    position--;
                    input[position] = '\0';

                    printf("\b \b");
                    fflush(stdout);
                }

                continue;
            }

            /* Store normal characters */
            if (position < BUFFER_SIZE - 1)
            {
                input[position++] = ch;

                putchar(ch);
                fflush(stdout);
            }
        }

        /* Handle exit command */
        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting shell...\n");
            break;
        }

        /* Handle empty input */
        if (strlen(input) == 0)
        {
            printf("No command entered.\n");
            continue;
        }

        printf("You entered: %s\n\n", input);
    }

    disableRawMode(&original);

    return 0;
}
