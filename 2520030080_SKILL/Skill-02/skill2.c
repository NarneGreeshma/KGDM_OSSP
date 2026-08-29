#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

#define INITIAL_BUFFER_SIZE 64
#define INITIAL_TOKEN_CAPACITY 8

/* =========================
   TOKEN DEFINITIONS
   ========================= */

typedef enum
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_INPUT,
    TOKEN_OUTPUT,
    TOKEN_SEMICOLON
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

typedef struct
{
    Token *tokens;
    size_t count;
    size_t capacity;
} TokenStream;

/* =========================
   COMMAND HISTORY
   ========================= */

typedef struct HistoryNode
{
    char *command;
    struct HistoryNode *prev;
    struct HistoryNode *next;
} HistoryNode;

static struct termios original_terminal;

/* =========================
   TERMINAL CONTROL
   ========================= */

void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO,
               TCSAFLUSH,
               &original_terminal);
}

void enable_raw_mode(void)
{
    tcgetattr(STDIN_FILENO,
              &original_terminal);

    struct termios raw = original_terminal;

    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO,
               TCSAFLUSH,
               &raw);

    atexit(disable_raw_mode);
}

/* =========================
   HISTORY MANAGEMENT
   ========================= */

HistoryNode *add_history(HistoryNode *tail,
                         const char *command)
{
    HistoryNode *node =
        malloc(sizeof(HistoryNode));

    if (node == NULL)
    {
        perror("malloc");
        return tail;
    }

    node->command =
        malloc(strlen(command) + 1);

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

void clear_line(size_t length)
{
    printf("\r\033[KOSSP> ");

    for (size_t i = 0; i < length; i++)
        putchar(' ');

    printf("\r\033[KOSSP> ");
    fflush(stdout);
}

/* =========================
   INPUT BUFFER
   ========================= */

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
            buffer[length] = '\0';
            putchar('\n');
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
         * Standard UP:
         * ESC [ A
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

            /* UP */
            if (b == 'A')
            {
                if (cursor != NULL)
                {
                    clear_line(length);

                    strcpy(buffer,
                           cursor->command);

                    length = strlen(buffer);

                    printf("%s", buffer);
                    fflush(stdout);

                    cursor = cursor->prev;
                }

                continue;
            }

            /* DOWN */
            if (b == 'B')
            {
                if (cursor != NULL &&
                    cursor->next != NULL)
                {
                    cursor = cursor->next;

                    clear_line(length);

                    strcpy(buffer,
                           cursor->command);

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

        /*
         * UTF-8 UP/DOWN support.
         *
         * UP   = E2 86 91
         * DOWN = E2 86 93
         */
        if (c == 0xE2)
        {
            unsigned char a, b;

            if (read(STDIN_FILENO, &a, 1) != 1)
                continue;

            if (read(STDIN_FILENO, &b, 1) != 1)
                continue;

            /* UTF-8 UP */
            if (a == 0x86 && b == 0x91)
            {
                if (cursor != NULL)
                {
                    clear_line(length);

                    strcpy(buffer,
                           cursor->command);

                    length = strlen(buffer);

                    printf("%s", buffer);
                    fflush(stdout);

                    cursor = cursor->prev;
                }

                continue;
            }

            /* UTF-8 DOWN */
            if (a == 0x86 && b == 0x93)
            {
                if (cursor != NULL &&
                    cursor->next != NULL)
                {
                    cursor = cursor->next;

                    clear_line(length);

                    strcpy(buffer,
                           cursor->command);

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

        /* Dynamic buffer resizing */
        if (length + 1 >= capacity)
        {
            size_t new_capacity =
                capacity * 2;

            char *new_buffer =
                realloc(buffer,
                        new_capacity);

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

/* =========================
   TOKEN FUNCTIONS
   ========================= */

const char *token_name(TokenType type)
{
    switch (type)
    {
        case TOKEN_WORD:
            return "WORD";

        case TOKEN_PIPE:
            return "PIPE";

        case TOKEN_INPUT:
            return "INPUT";

        case TOKEN_OUTPUT:
            return "OUTPUT";

        case TOKEN_SEMICOLON:
            return "SEMICOLON";

        default:
            return "UNKNOWN";
    }
}

void init_token_stream(TokenStream *stream)
{
    stream->count = 0;
    stream->capacity =
        INITIAL_TOKEN_CAPACITY;

    stream->tokens =
        malloc(stream->capacity *
               sizeof(Token));

    if (stream->tokens == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

void add_token(TokenStream *stream,
               TokenType type,
               const char *value)
{
    if (stream->count >= stream->capacity)
    {
        size_t new_capacity =
            stream->capacity * 2;

        Token *new_tokens =
            realloc(stream->tokens,
                    new_capacity *
                    sizeof(Token));

        if (new_tokens == NULL)
        {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        stream->tokens = new_tokens;
        stream->capacity = new_capacity;
    }

    stream->tokens[stream->count].type =
        type;

    stream->tokens[stream->count].value =
        malloc(strlen(value) + 1);

    if (stream->tokens[stream->count].value == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(stream->tokens[stream->count].value,
           value);

    stream->count++;
}

void free_token_stream(TokenStream *stream)
{
    for (size_t i = 0;
         i < stream->count;
         i++)
    {
        free(stream->tokens[i].value);
    }

    free(stream->tokens);

    stream->tokens = NULL;
    stream->count = 0;
    stream->capacity = 0;
}

/* =========================
   TOKENIZER
   ========================= */

int is_delimiter(char c)
{
    return c == '|' ||
           c == '<' ||
           c == '>' ||
           c == ';';
}

void tokenize(const char *input,
              TokenStream *stream)
{
    size_t i = 0;

    while (input[i] != '\0')
    {
        /* Handle whitespace */
        if (isspace((unsigned char)input[i]))
        {
            i++;
            continue;
        }

        /* Handle delimiters */
        if (is_delimiter(input[i]))
        {
            TokenType type;

            if (input[i] == '|')
                type = TOKEN_PIPE;
            else if (input[i] == '<')
                type = TOKEN_INPUT;
            else if (input[i] == '>')
                type = TOKEN_OUTPUT;
            else
                type = TOKEN_SEMICOLON;

            char delimiter[2];

            delimiter[0] = input[i];
            delimiter[1] = '\0';

            add_token(stream,
                      type,
                      delimiter);

            i++;
            continue;
        }

        /* Word token */
        size_t capacity = 32;
        size_t length = 0;

        char *word = malloc(capacity);

        if (word == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        while (input[i] != '\0' &&
               !isspace((unsigned char)input[i]) &&
               !is_delimiter(input[i]))
        {
            if (length + 1 >= capacity)
            {
                capacity *= 2;

                char *new_word =
                    realloc(word,
                            capacity);

                if (new_word == NULL)
                {
                    perror("realloc");
                    free(word);
                    exit(EXIT_FAILURE);
                }

                word = new_word;
            }

            word[length++] = input[i++];
        }

        word[length] = '\0';

        add_token(stream,
                  TOKEN_WORD,
                  word);

        free(word);
    }
}

/* =========================
   TOKEN VALIDATION
   ========================= */

int validate_tokens(const TokenStream *stream)
{
    if (stream->count == 0)
        return 1;

    /* Cannot begin with pipe */
    if (stream->tokens[0].type ==
        TOKEN_PIPE)
    {
        printf("Syntax Error: command cannot start with '|'.\n");
        return 0;
    }

    for (size_t i = 0;
         i < stream->count;
         i++)
    {
        TokenType current =
            stream->tokens[i].type;

        /* Pipe validation */
        if (current == TOKEN_PIPE)
        {
            if (i + 1 >= stream->count)
            {
                printf("Syntax Error: command cannot end with '|'.\n");
                return 0;
            }

            if (stream->tokens[i + 1].type ==
                TOKEN_PIPE)
            {
                printf("Syntax Error: consecutive pipes are not allowed.\n");
                return 0;
            }
        }

        /* Redirection validation */
        if (current == TOKEN_INPUT ||
            current == TOKEN_OUTPUT)
        {
            if (i + 1 >= stream->count ||
                stream->tokens[i + 1].type !=
                TOKEN_WORD)
            {
                printf("Syntax Error: redirection requires a filename.\n");
                return 0;
            }
        }
    }

    return 1;
}

/* =========================
   PARSE TREE
   ========================= */

typedef struct ParseNode
{
    TokenType type;
    char *value;

    struct ParseNode *left;
    struct ParseNode *right;
} ParseNode;

ParseNode *create_parse_node(TokenType type,
                             const char *value)
{
    ParseNode *node =
        malloc(sizeof(ParseNode));

    if (node == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    node->type = type;

    node->value =
        malloc(strlen(value) + 1);

    if (node->value == NULL)
    {
        perror("malloc");
        free(node);
        exit(EXIT_FAILURE);
    }

    strcpy(node->value, value);

    node->left = NULL;
    node->right = NULL;

    return node;
}

void free_parse_tree(ParseNode *node)
{
    if (node == NULL)
        return;

    free_parse_tree(node->left);
    free_parse_tree(node->right);

    free(node->value);
    free(node);
}

/*
 * Build a simple execution structure.
 *
 * PIPE nodes connect commands.
 * WORD nodes represent command arguments.
 */
ParseNode *build_parse_tree(const TokenStream *stream)
{
    if (stream->count == 0)
        return NULL;

    ParseNode *root = NULL;
    ParseNode *current = NULL;

    for (size_t i = 0;
         i < stream->count;
         i++)
    {
        const Token *token =
            &stream->tokens[i];

        if (token->type == TOKEN_PIPE)
        {
            ParseNode *pipe =
                create_parse_node(TOKEN_PIPE,
                                  "|");

            pipe->left = root;
            root = pipe;
            current = NULL;

            continue;
        }

        ParseNode *node =
            create_parse_node(token->type,
                              token->value);

        if (root == NULL)
        {
            root = node;
            current = root;
        }
        else if (root->type == TOKEN_PIPE &&
                 root->right == NULL)
        {
            root->right = node;
            current = node;
        }
        else if (current != NULL)
        {
            if (current->left == NULL)
            {
                current->left = node;
            }
            else if (current->right == NULL)
            {
                current->right = node;
            }
            else
            {
                ParseNode *temp =
                    current->right;

                while (temp->right != NULL)
                    temp = temp->right;

                temp->right = node;
            }
        }
    }

    return root;
}

void print_parse_tree(ParseNode *node,
                       int depth)
{
    if (node == NULL)
        return;

    for (int i = 0;
         i < depth;
         i++)
    {
        printf("  ");
    }

    printf("|-- %s: %s\n",
           token_name(node->type),
           node->value);

    print_parse_tree(node->left,
                     depth + 1);

    print_parse_tree(node->right,
                     depth + 1);
}

/* =========================
   MAIN
   ========================= */

int main(void)
{
    HistoryNode *head = NULL;
    HistoryNode *tail = NULL;

    printf("============================================\n");
    printf("       OSSP SKILL-02: HISTORY & PARSER\n");
    printf("============================================\n");

    printf("Features:\n");
    printf("- Command history using linked lists\n");
    printf("- UP/DOWN history navigation\n");
    printf("- Escape sequence handling\n");
    printf("- Dynamic input buffers\n");
    printf("- Dynamic token arrays\n");
    printf("- Tokenization and delimiter detection\n");
    printf("- Token stream validation\n");
    printf("- Parse tree generation\n");
    printf("- Syntax error detection\n");
    printf("- Empty command handling\n");
    printf("- Memory cleanup\n\n");

    enable_raw_mode();

    while (1)
    {
        char *input =
            read_input(tail);

        if (input == NULL)
            break;

        /* Empty command */
        if (strlen(input) == 0)
        {
            printf("Empty command: nothing to parse.\n\n");

            free(input);
            continue;
        }

        /* Exit */
        if (strcmp(input, "exit") == 0)
        {
            free(input);
            break;
        }

        /* Store command in history */
        HistoryNode *new_tail =
            add_history(tail,
                        input);

        if (head == NULL)
            head = new_tail;

        tail = new_tail;

        printf("Command stored: %s\n",
               input);

        /* Tokenization */
        TokenStream stream;

        init_token_stream(&stream);

        tokenize(input,
                 &stream);

        printf("\n--- TOKEN STREAM ---\n");

        for (size_t i = 0;
             i < stream.count;
             i++)
        {
            printf("[%zu] %-10s %s\n",
                   i,
                   token_name(
                       stream.tokens[i].type),
                   stream.tokens[i].value);
        }

        /* Validation and parsing */
        printf("\n--- TOKEN VALIDATION ---\n");

        if (validate_tokens(&stream))
        {
            printf("Token stream is valid.\n");

            printf("\n--- PARSE TREE ---\n");

            ParseNode *tree =
                build_parse_tree(&stream);

            print_parse_tree(tree, 0);

            free_parse_tree(tree);
        }
        else
        {
            printf("Token stream is invalid.\n");
        }

        free_token_stream(&stream);
        free(input);

        printf("\n");
    }

    disable_raw_mode();

    free_history(head);

    printf("\nHistory and parser memory released successfully.\n");
    printf("Program terminated.\n");

    return 0;
}
