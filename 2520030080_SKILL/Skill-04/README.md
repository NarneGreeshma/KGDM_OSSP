# OSSP SKILL-04

## Title

Shell Extensions: Process Synchronization, PATH Resolution, Variable Expansion and Built-ins

## Objective

To implement process synchronization, command resolution using the PATH variable, environment variable expansion, and built-in command dispatch in C.

## Requirements Covered

### Process Synchronization

- Monitor child processes
- Create child processes using `fork()`
- Synchronize parent and child using `waitpid()`

### PATH Resolution

- Retrieve the PATH environment variable
- Parse PATH search directories
- Locate executable commands
- Verify execute permissions using `access()`
- Handle missing commands
- Test command resolution

### Variable Expansion

- Detect variable references such as `$HOME`
- Expand environment variable values
- Handle undefined variables
- Update command tokens after expansion
- Test variable expansion logic

### Built-in Commands

- Identify built-in commands
- Use a dispatch table
- Execute built-ins inside the shell process
- Handle invalid commands
- Maintain shell state

Supported built-ins:

- `cd`
- `pwd`
- `help`
- `exit`

## Program Flow

    User enters command
            |
            v
    Variable Expansion
            |
            v
        Tokenization
            |
            v
      Built-in check
         /       \
        /         \
   Built-in      External
      |             |
   Execute       PATH search
   in parent         |
                     v
                   fork()
                  /     \
              Parent    Child
                |         |
             waitpid()  execv()
                |
                v
           Next prompt

## PATH Resolution

The program obtains PATH using:

    getenv("PATH")

The PATH is split into individual directories and each directory is searched for the requested executable.

Executable permission is verified using:

    access(path, X_OK)

If no executable is found, the program reports:

    command not found

## Variable Expansion

Examples:

    OSSP> echo $HOME

    OSSP> echo $USER

Undefined variables are expanded to an empty value.

## Built-in Dispatch

Built-in commands are identified using a dispatch table.

For example:

    cd /tmp
    pwd

The `cd` command executes inside the parent shell process, allowing the changed directory to persist for subsequent commands.

## Process Synchronization

External commands are executed using a child process.

The parent waits for the child using:

    waitpid()

This ensures proper child-process monitoring and synchronization.

## Compilation

Compile using:

    make

Or:

    gcc -Wall -Wextra -g skill4.c -o skill4

## Execution

    ./skill4

## Memory Verification

Valgrind can be used to verify memory management:

    valgrind --leak-check=full --track-origins=yes ./skill4

## Result

A shell extension was successfully implemented demonstrating `waitpid()` synchronization, PATH-based executable resolution, variable expansion, built-in command dispatch, shell-state maintenance, and invalid-command handling.
