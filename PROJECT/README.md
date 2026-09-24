# Multithreaded Linux Application Using POSIX Threads and Mutexes

<p align="center">
  <b>Operating Systems and Systems Programming Project</b>
</p>

<p align="center">
  A practical Linux application demonstrating multithreading, synchronization,
  producer-consumer architecture, task queues, and graceful shutdown.
</p>

---

## 📌 Project Overview

This project implements a **Multithreaded Linux Application** using the **C programming language** and **POSIX Threads (pthreads)**.

The application follows the **Producer-Consumer model**.

The main thread acts as the **producer**. It reads integer values from an input file and inserts them into a shared circular task queue.

Multiple worker threads act as **consumers**. They retrieve tasks from the queue, process them concurrently, and write the results to an output file.

The project demonstrates important Operating System concepts including:

- POSIX Threads
- Multithreading
- Mutex synchronization
- Condition variables
- Producer-Consumer model
- Shared resources
- Circular queues
- Signal handling
- Graceful thread termination
- Thread joining
- File-based input/output

---

# 🎯 Problem Statement

When multiple threads access shared data simultaneously, unsafe access can result in **race conditions**, inconsistent data, and unexpected program behavior.

The objective of this project is to implement a safe and synchronized task-processing system where:

- The main thread produces tasks.
- Worker threads consume tasks.
- A shared circular queue stores pending tasks.
- A mutex protects shared queue operations.
- A condition variable coordinates worker threads.
- Tasks are processed concurrently.
- Results are stored in an output file.
- `SIGINT` is used for graceful shutdown.

---

# 🎯 Objectives

1. Implement multithreading using POSIX Threads.
2. Create and manage multiple worker threads.
3. Implement a shared circular task queue.
4. Synchronize shared resources using mutexes.
5. Use condition variables for thread coordination.
6. Implement the Producer-Consumer model.
7. Process multiple tasks concurrently.
8. Store processed results in an output file.
9. Implement graceful shutdown using `SIGINT`.
10. Demonstrate practical Operating System concepts in Linux.

---

# 🏗️ System Architecture

```text
                         INPUT
                    data/tasks.txt
                           │
                           ▼
                  ┌─────────────────┐
                  │   MAIN THREAD   │
                  │    PRODUCER     │
                  └────────┬────────┘
                           │
                           ▼
                  ┌─────────────────┐
                  │  SHARED TASK    │
                  │      QUEUE       │
                  │   Circular FIFO  │
                  └────────┬────────┘
                           │
                 Mutex + Condition Variable
                           │
              ┌────────────┼────────────┐
              │            │            │
              ▼            ▼            ▼
         ┌─────────┐  ┌─────────┐  ┌─────────┐
         │ Worker 1│  │ Worker 2│  │ Worker 3│
         └────┬────┘  └────┬────┘  └────┬────┘
              │            │            │
              └────────────┼────────────┘
                           │
                           ▼
                    TASK PROCESSING
                           │
                           ▼
                     value × value
                           │
                           ▼
                  output/results.txt
