# OSSP SKILL-01

## Title

Mini Shell: Process Management and Interactive Input

## Objective

To understand Linux process management and implement a basic interactive shell using C.

## Requirements Covered

### Environment and Shell Architecture

- Linux environment
- GCC configuration
- Git repository setup
- Project structure
- Shell architecture
- Makefile-based compilation

### Process Management

- Process abstraction
- `fork()` system call
- `exec()` family
- Parent-child process relationships
- Process identification using PID and PPID
- Process synchronization using `waitpid()`
- Process tree inspection
- System-call tracing using `strace`

### Interactive Shell

- Main interactive loop
- Shell prompt
- Reading user input
- Exit condition
- Keyboard input
- Backspace handling
- Enter-key processing
- Dynamic input buffer
- Multi-character commands
- Interactive command execution

## Program Description

The program implements a simple interactive shell with the prompt:

    OSSP>

The shell accepts commands from the user and creates a child process using `fork()`.

The child process executes the requested command using `execvp()`.

The parent process waits for the child using `waitpid()` before displaying the next prompt.

## Process Flow

    User enters command
            |
            v
      Read input
            |
            v
        fork()
       /      \
      /        \
   Parent      Child
     |           |
     |        execvp()
     |           |
     |      Execute command
     |           |
     +---- waitpid()
            |
            v
       Next prompt

## Input Handling

The program uses a dynamically allocated input buffer.

The buffer is resized using `realloc()` when additional space is required.

Backspace is handled to allow the user to correct input.

The Enter key completes the current command.

Multiple words can be entered as a single command.

## Example

    OSSP> ls

    [Parent]
    PID       : 849
    Child PID : 850

    [Child]
    PID  : 850
    PPID : 849

    Executing command using execvp()...

    Child 850 exited with status 0.

    OSSP> echo hello world

    hello world

## Process Tree Inspection

The process hierarchy can be inspected using:

    ps -o pid,ppid,stat,cmd --forest

and:

    pstree -p

These commands show the relationship between the shell process and its parent process.

## System Call Tracing

System calls can be traced using:

    strace -f -o trace.txt ./skill1

Important system calls include:

- `clone()` - Linux process creation mechanism used underneath `fork()`
- `execve()` - execution of a new program
- `read()` - reading user input
- `write()` - displaying output
- `wait4()` - underlying system call associated with `waitpid()`

The `-f` option traces system calls from child processes as well.

## Compilation

Compile using:

    make

Or directly:

    gcc -Wall -Wextra -g skill1.c -o skill1

## Execution

    ./skill1

## Cleaning Build Files

    make clean

## Result

A functional mini shell was successfully implemented using C.

The program demonstrates process creation, process execution, parent-child relationships, process synchronization, interactive input handling, dynamic input buffering, process-tree inspection, and system-call tracing.
