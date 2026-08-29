# OSSP SKILL-02

## Title

Command History, Dynamic Memory, Tokenization and Parsing

## Objective

To implement command history management, dynamic memory handling, tokenization, syntax validation, and parser structures in C.

## Requirements Covered

### Command History and Dynamic Memory

- Apply terminal escape sequences
- Store command history
- Navigate previous commands using the UP arrow
- Navigate next commands using the DOWN arrow
- Update the input buffer during history recall
- Test command recall functionality
- Allocate buffers dynamically using `malloc()`
- Resize buffers using `realloc()`
- Prevent buffer overflow through dynamic resizing
- Manage command history using a doubly linked list
- Release allocated memory using `free()`
- Verify memory management using Valgrind

### Tokenization

- Split input into tokens
- Identify delimiters
- Handle whitespace
- Create dynamically allocated token structures
- Validate token streams
- Display tokenization results for debugging

Supported delimiters:

- `|`
- `<`
- `>`
- `;`

### Parsing

- Design parser logic
- Generate a parse tree
- Validate command syntax
- Detect syntax errors
- Handle empty commands
- Produce an execution structure from tokens

## Program Description

The program implements an interactive command processor.

Commands entered by the user are stored in a dynamically managed command-history linked list.

The UP and DOWN escape sequences allow previously entered commands to be recalled and placed back into the input buffer.

After a command is entered, the tokenizer identifies words, whitespace-separated elements, and shell delimiters.

The resulting token stream is validated before a parse tree is generated.

## History Flow

    User enters command
            |
            v
      Dynamic input buffer
            |
            v
       Store in history
            |
            v
       Doubly linked list
          /       \
        UP         DOWN
        |            |
        +---- Recall-+

## Tokenization Flow

    Input command
          |
          v
     Read characters
          |
          v
   Identify whitespace
     and delimiters
          |
          v
      Create tokens
          |
          v
     Validate tokens

## Parsing Flow

    Valid token stream
            |
            v
      Parser logic
            |
            v
       Parse tree
            |
            v
    Execution structure

## Example

    OSSP> echo hello | grep hello

    --- TOKEN STREAM ---
    [0] WORD       echo
    [1] WORD       hello
    [2] PIPE       |
    [3] WORD       grep
    [4] WORD       hello

    --- TOKEN VALIDATION ---
    Token stream is valid.

    --- PARSE TREE ---
    |-- PIPE: |
      |-- WORD: echo
      |-- WORD: hello

## Syntax Error Example

    OSSP> echo hello |

    Syntax Error: command cannot end with '|'.

## Empty Command

Pressing Enter without entering a command produces:

    Empty command: nothing to parse.

## Memory Management

Dynamic memory is used for:

- Input buffers
- History nodes
- History command strings
- Token arrays
- Token strings
- Parse-tree nodes

All allocated memory is released before program termination.

## Valgrind Verification

Run:

    valgrind --leak-check=full --track-origins=yes ./skill2

The program should terminate with no memory leaks and no memory errors.

## Compilation

Compile using:

    make

Or directly:

    gcc -Wall -Wextra -g skill2.c -o skill2

## Execution

    ./skill2

## Cleaning Build Files

    make clean

## Result

A command-history and parsing system was successfully implemented using dynamic memory allocation, linked lists, tokenization, syntax validation, and parse-tree generation.
