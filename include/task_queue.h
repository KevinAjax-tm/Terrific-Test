#pragma once
#include "common.h"
#include <stdbool.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE 1024

typedef struct {
    Task tasks[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} TaskQueue;

int task_queue_init(TaskQueue *queue);
void task_queue_destroy(TaskQueue *queue);
bool task_queue_push(TaskQueue *queue, const Task *task);
bool task_queue_pop(TaskQueue *queue, Task *out_task);
bool task_queue_is_empty(TaskQueue *queue);
bool task_queue_is_full(TaskQueue *queue);