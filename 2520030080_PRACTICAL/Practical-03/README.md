# OSSP Practical-03

## Title
Process Creation and Process State Monitoring Using Linux

## Aim
To develop a C program using `fork()` to create parent and child processes, display their PID and PPID, and observe process states using Linux monitoring tools.

## Objectives

- Create a child process using `fork()`.
- Display PID and PPID of parent and child processes.
- Observe process states using `/proc`.
- Monitor processes using `ps` and `top`.
- Observe the parent-child relationship.
- Study process termination.

## Tools Used

- GCC
- Linux / WSL
- `ps`
- `top`
- `/proc`
- `pstree`

## Process State Observation

The child process is placed into a sleeping state using `sleep()`. Its actual state is inspected through `/proc/<PID>/status` and monitored using `ps` and `top`.

## Result

The parent and child processes were successfully created using `fork()`. Their PID, PPID and process state were observed using Linux process-monitoring tools. The child process was observed in the sleeping state and its termination was verified after execution.

## Conclusion

The practical demonstrated process creation using `fork()` and provided hands-on understanding of Linux process states, parent-child relationships, process monitoring and termination.
