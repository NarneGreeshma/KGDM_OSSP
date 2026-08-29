# OSSP SKILL-03

## Title

Quoting, Escape Sequences and Command Execution

## Objective

To implement shell-style single quotes, double quotes, escape sequences, and command execution using child processes.

## Requirements Covered

### Single Quotes

- Apply single quotes
- Preserve literal content
- Ignore variable expansion inside single quotes
- Store quoted strings as tokens
- Validate parsing results
- Test edge cases

Example:

    echo '$HOME'

The `$HOME` text is preserved literally.

### Double Quotes

- Apply double quotes
- Preserve spaces inside quoted strings
- Allow variable expansion
- Parse quoted tokens
- Validate outputs
- Test quoted commands

Example:

    echo "Home is $HOME"

The `$HOME` variable is expanded.

### Escape Sequences

- Handle escaped spaces
- Escape special symbols
- Preserve escaped characters
- Validate parser output
- Test complex inputs

Example:

    echo hello\ world

The escaped space keeps `hello world` together as one token.

### Process Execution

- Create child processes using `fork()`
- Execute programs using `execvp()`
- Handle execution errors
- Pass command arguments
- Manage the parent process
- Wait for child completion
- Test command launching

## Program Flow

    User Input
        |
        v
    Tokenization
        |
        +------------------+
        |                  |
        v                  v
    Single Quotes      Double Quotes
        |                  |
        |             Variable Expansion
        |                  |
        +--------+---------+
                 |
                 v
        Escape Sequence Handling
                 |
                 v
           Token Stream
                 |
                 v
              fork()
              /    \
             /      \
        Parent       Child
          |            |
       waitpid()     execvp()
          |            |
          +-----+------+
                |
                v
          Command Result

## Examples

### Single-Quoted String

    OSSP> echo 'hello world'

    [0] "echo"
    [1] "hello world"

### Literal Variable

    OSSP> echo '$HOME'

    [0] "echo"
    [1] "$HOME"

### Double-Quoted String

    OSSP> echo "hello world"

    [0] "echo"
    [1] "hello world"

### Variable Expansion

    OSSP> echo "Home is $HOME"

The value of `HOME` is substituted.

### Escaped Space

    OSSP> echo hello\ world

The escaped space is preserved in the token.

## Process Management

For a valid command, the parent process creates a child using `fork()`.

The child executes the requested program using:

    execvp()

The parent waits for the child using:

    waitpid()

Execution errors are reported using `perror()`.

## Error Handling

The program detects:

- Unmatched single quotes
- Unmatched double quotes
- Trailing escape characters
- Too many arguments
- Memory allocation failures
- `fork()` failures
- `execvp()` failures

## Empty Commands

Pressing Enter without a command produces:

    Empty command.

## Compilation

Using Makefile:

    make

Direct compilation:

    gcc -Wall -Wextra -g skill3.c -o skill3

## Execution

    ./skill3

## Memory Verification

Run:

    valgrind --leak-check=full --track-origins=yes ./skill3

The program should terminate without memory leaks or memory errors.

## Cleaning

    make clean

## Result

Single quotes, double quotes, escape sequences, token parsing, and process execution were successfully implemented and tested using C system calls and dynamic memory management.
