#define _POSIX_C_SOURCE 200809L  // For shm_open
#include "../include/ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// ... rest of ipc.c implementation ...
SharedResults* ipc_init() {
    // Create shared memory object
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        return NULL;
    }

    // Set size
    if (ftruncate(fd, sizeof(SharedResults)) == -1) {
        perror("ftruncate");
        close(fd);
        return NULL;
    }

    // Memory mapping
    SharedResults* shmem = mmap(NULL, sizeof(SharedResults), 
                               PROT_READ | PROT_WRITE, 
                               MAP_SHARED, fd, 0);
    close(fd);

    if (shmem == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    // Initialize mutex with process-shared attribute
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shmem->lock, &attr);

    memset(shmem->results, 0, sizeof(shmem->results));
    return shmem;
}

void ipc_store_result(SharedResults* shmem, const TaskResult* result) {
    pthread_mutex_lock(&shmem->lock);
    
    // Find empty slot (in real code, use circular buffer)
    for (int i = 0; i < MAX_RESULTS; i++) {
        if (shmem->results[i].task_id == 0) {
            shmem->results[i] = *result;
            break;
        }
    }
    
    pthread_mutex_unlock(&shmem->lock);
}

void ipc_cleanup(SharedResults* shmem) {
    munmap(shmem, sizeof(SharedResults));
    shm_unlink(SHM_NAME);
}