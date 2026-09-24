#include <stdio.h>
#include "task_queue.h"

void init_queue(TaskQueue *queue)
{
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
}

void add_task(TaskQueue *queue, Task task)
{
    if (queue->count < QUEUE_SIZE)
    {
        queue->tasks[queue->rear] = task;
        queue->rear = (queue->rear + 1) % QUEUE_SIZE;
        queue->count++;
    }
}

Task get_task(TaskQueue *queue)
{
    Task task = queue->tasks[queue->front];

    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->count--;

    return task;
}
