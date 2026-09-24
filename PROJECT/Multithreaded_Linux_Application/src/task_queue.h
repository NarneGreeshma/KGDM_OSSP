#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#define QUEUE_SIZE 10

typedef struct
{
    int task_id;
    int value;
} Task;

typedef struct
{
    Task tasks[QUEUE_SIZE];
    int front;
    int rear;
    int count;
} TaskQueue;

void init_queue(TaskQueue *queue);
void add_task(TaskQueue *queue, Task task);
Task get_task(TaskQueue *queue);

#endif
