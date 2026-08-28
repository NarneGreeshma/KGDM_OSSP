# OSSP SKILL-03

## Title
Command History and Dynamic Memory Management

## Objectives

- Handle keyboard escape sequences.
- Store command history.
- Navigate previous and next commands using arrow keys.
- Update the input buffer during history navigation.
- Dynamically allocate and resize input buffers.
- Prevent buffer overflow.
- Manage command history using a doubly linked list.
- Release allocated memory correctly.
- Verify memory management using Valgrind.

## Features

- UP arrow recalls previous commands.
- DOWN arrow navigates toward newer commands.
- Backspace is supported.
- Input buffer grows dynamically using `realloc()`.
- Command history is maintained using a doubly linked list.
- Allocated memory is released using `free()`.

## Memory Verification

Valgrind was used with:

`valgrind --leak-check=full --track-origins=yes ./skill3`

The program was checked for memory leaks and invalid memory operations.

## Result

Command history navigation, dynamic buffer management, linked-list history and memory cleanup were successfully implemented and tested.
