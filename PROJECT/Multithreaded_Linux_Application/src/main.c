#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "task_queue.h"

TaskQueue queue;

pthread_mutex_t queue_mutex;
pthread_cond_t queue_condition;

FILE *output_file;

int production_finished = 0;

volatile sig_atomic_t shutdown_requested = 0;

/* Signal handler */
void handle_signal(int signal)
{
    if (signal == SIGINT)
    {
        shutdown_requested = 1;
    }
}

/* Worker thread */
void *worker(void *arg)
{
    int id = *(int *)arg;

    while (!shutdown_requested)
    {
        pthread_mutex_lock(&queue_mutex);

        while (queue.count == 0 &&
               !production_finished &&
               !shutdown_requested)
        {
            pthread_cond_wait(&queue_condition, &queue_mutex);
        }

        if (shutdown_requested)
        {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        if (queue.count == 0 && production_finished)
        {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        Task task = get_task(&queue);

        pthread_mutex_unlock(&queue_mutex);

        int result = task.value * task.value;

        pthread_mutex_lock(&queue_mutex);

        fprintf(output_file,
                "Worker %d processed Task %d: %d -> %d\n",
                id,
                task.task_id,
                task.value,
                result);

        fflush(output_file);

        pthread_mutex_unlock(&queue_mutex);

        printf("Worker %d processed Task %d: %d -> %d\n",
               id,
               task.task_id,
               task.value,
               result);

        sleep(1);
    }

    printf("Worker %d stopped.\n", id);

    return NULL;
}

int main()
{
    pthread_t workers[3];
    int worker_ids[3];

    FILE *input_file;

    /* Register SIGINT handler */
    signal(SIGINT, handle_signal);

    printf("Multithreaded Linux Application\n");
    printf("Press Ctrl+C to request shutdown.\n\n");

    /* Open input file */
    input_file = fopen("data/tasks.txt", "r");

    if (input_file == NULL)
    {
        perror("Unable to open input file");
        return 1;
    }

    /* Open output file */
    output_file = fopen("output/results.txt", "w");

    if (output_file == NULL)
    {
        perror("Unable to open output file");
        fclose(input_file);
        return 1;
    }

    /* Initialize synchronization */
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_condition, NULL);

    /* Initialize task queue */
    init_queue(&queue);

    /* Create worker threads */
    for (int i = 0; i < 3; i++)
    {
        worker_ids[i] = i + 1;

        pthread_create(
            &workers[i],
            NULL,
            worker,
            &worker_ids[i]
        );
    }

    /* Read tasks from file */
    int value;
    int task_id = 1;

    while (fscanf(input_file, "%d", &value) == 1)
    {
        if (shutdown_requested)
        {
            break;
        }

        Task task;

        task.task_id = task_id++;
        task.value = value;

        pthread_mutex_lock(&queue_mutex);

        add_task(&queue, task);

        pthread_cond_signal(&queue_condition);

        pthread_mutex_unlock(&queue_mutex);

        sleep(1);
    }

    fclose(input_file);

    /* Tell workers that production is finished */
    pthread_mutex_lock(&queue_mutex);

    production_finished = 1;

    pthread_cond_broadcast(&queue_condition);

    pthread_mutex_unlock(&queue_mutex);

    /* Wait for all workers */
    for (int i = 0; i < 3; i++)
    {
        pthread_join(workers[i], NULL);
    }

    fclose(output_file);

    /* Destroy synchronization objects */
    pthread_cond_destroy(&queue_condition);
    pthread_mutex_destroy(&queue_mutex);

    if (shutdown_requested)
    {
        printf("\nShutdown requested by user.\n");
    }
    else
    {
        printf("\nAll tasks completed.\n");
    }

    printf("Results saved to output/results.txt\n");

    return 0;
}
