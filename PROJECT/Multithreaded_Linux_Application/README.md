# Multithreaded Linux Application Using POSIX Threads and Mutexes

## Project Overview

This project is a Multithreaded Linux Application developed using the C programming language and POSIX Threads.

The application follows a Producer-Consumer model, where the main thread reads tasks from an input file and adds them to a shared task queue. Three worker threads retrieve and process the tasks concurrently.

The project demonstrates important Operating Systems concepts including process/thread creation, thread synchronization, mutexes, condition variables, shared resources, signal handling, and graceful thread termination.

## Problem Statement

In a multithreaded application, multiple worker threads may need to access the same shared data simultaneously. Without proper synchronization, this can lead to race conditions and inconsistent results.

This project implements a thread-safe task processing system where:

- The main thread produces tasks.
- Multiple worker threads consume tasks.
- A shared circular queue stores pending tasks.
- Mutexes protect shared resources.
- A condition variable coordinates worker threads.
- SIGINT handling provides graceful shutdown.
- Processed results are stored in an output file.

## Objectives

- Implement multithreading using POSIX Threads.
- Create and manage multiple worker threads.
- Implement a shared circular task queue.
- Synchronize access to shared resources using mutexes.
- Use condition variables for thread coordination.
- Process multiple tasks concurrently.
- Store processing results in a file.
- Implement graceful shutdown using SIGINT.
- Demonstrate practical Operating Systems concepts in Linux.

## Technologies Used

- Programming Language: C
- Operating System: Linux / WSL2
- Compiler: GCC
- Threading: POSIX Threads (pthread)
- Synchronization: Mutexes and Condition Variables
- Signal Handling: SIGINT
- Version Control: Git and GitHub

## Project Structure

```text
Multithreaded_Linux_Application/
│
├── src/
│   ├── main.c
│   ├── task_queue.c
│   └── task_queue.h
│
├── data/
│   └── tasks.txt
│
├── output/
│   └── results.txt
│
└── README.md
