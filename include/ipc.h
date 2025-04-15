#pragma once
#include "common.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_NAME "/distask_results"
#define MAX_RESULTS 1000

typedef struct {
    TaskResult results[MAX_RESULTS];
    pthread_mutex_t lock;
} SharedResults;

SharedResults* ipc_init();
void ipc_store_result(SharedResults* shmem, const TaskResult* result);
void ipc_cleanup(SharedResults* shmem);