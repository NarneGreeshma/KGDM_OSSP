# OSSP Practical-04

## Title
Process Synchronization and Zombie Process Management

## Aim
To create multiple child processes using `fork()`, synchronize their completion using `wait()` and `waitpid()`, compare their behavior, and study zombie processes.

## Objectives

- Create multiple child processes using `fork()`.
- Synchronize child processes using `wait()`.
- Synchronize a specific child using `waitpid()`.
- Create and observe a zombie process.
- Inspect the zombie using `ps` and `/proc`.
- Eliminate the zombie using `waitpid()`.

## wait() vs waitpid()

- `wait()` waits for any child process to terminate.
- `waitpid()` can wait for a specific child process identified by its PID.

## Zombie Process Observation

A child process was intentionally terminated while the parent continued executing without immediately collecting its exit status. The process was observed using `ps` with state `Z` and `/proc/<PID>/status` with:

`State: Z (zombie)`

## Zombie Elimination

The parent subsequently used `waitpid()` to collect the terminated child's status. After synchronization, the zombie process was removed from the process table.

## Result

Multiple child processes were successfully created and synchronized. The behavior of `wait()` and `waitpid()` was observed, and a zombie process was successfully created, investigated, and eliminated using proper synchronization.

## Conclusion

The practical demonstrated process synchronization using `wait()` and `waitpid()` and provided practical understanding of zombie process creation, observation, and elimination in Linux.
