#pragma once
#include "common.h"
#include "task_queue.h"
#include "ipc.h"

typedef struct {
    pthread_t thread;
    int id;
    bool active;
    TaskQueue *queue;
    SharedResults *shmem;
} Worker;

typedef struct {
    Worker *workers;
    int max_threads;
    pthread_mutex_t lock;
} ThreadPool;

int thread_pool_init(ThreadPool *pool, int max_threads, TaskQueue *queue, SharedResults *shmem);
void thread_pool_shutdown(ThreadPool *pool);