#include "../include/thread_pool.h"
#include "../include/subprocess.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>  // Add this for strerror()

static void* worker_thread(void *arg) {
    Worker *worker = (Worker*)arg;
    Task task;
    TaskResult result;

    while (1) {
        if (!task_queue_pop(worker->queue, &task)) {
            fprintf(stderr, "Worker %d: Failed to pop task\n", worker->id);
            break;
        }

        Subprocess proc = {
            .argv = task.argv,
            .timeout_sec = task.timeout_sec
        };

        result = (TaskResult){
            .task_id = task.task_id,
            .exit_code = run_subprocess(&proc),
            .exec_time = 0.0
        };

        ipc_store_result(worker->shmem, &result);
    }
    return NULL;
}

int thread_pool_init(ThreadPool *pool, int max_threads, TaskQueue *queue, SharedResults *shmem) {
    pool->workers = malloc(max_threads * sizeof(Worker));
    if (!pool->workers) {
        fprintf(stderr, "Failed to allocate workers: %s\n", strerror(errno));
        return -1;
    }

    if (pthread_mutex_init(&pool->lock, NULL) != 0) {
        free(pool->workers);
        fprintf(stderr, "Mutex init failed: %s\n", strerror(errno));
        return -1;
    }

    for (int i = 0; i < max_threads; i++) {
        pool->workers[i].id = i;
        pool->workers[i].active = true;
        pool->workers[i].queue = queue;
        pool->workers[i].shmem = shmem;

        if (pthread_create(&pool->workers[i].thread, NULL, worker_thread, &pool->workers[i]) != 0) {
            fprintf(stderr, "Failed to create thread %d: %s\n", i, strerror(errno));
            for (int j = 0; j < i; j++) {
                pthread_cancel(pool->workers[j].thread);
            }
            free(pool->workers);
            pthread_mutex_destroy(&pool->lock);
            return -1;
        }
    }

    pool->max_threads = max_threads;
    return 0;
}

void thread_pool_shutdown(ThreadPool *pool) {
    for (int i = 0; i < pool->max_threads; i++) {
        pthread_cancel(pool->workers[i].thread);
        pthread_join(pool->workers[i].thread, NULL);
    }
    free(pool->workers);
    pthread_mutex_destroy(&pool->lock);
}