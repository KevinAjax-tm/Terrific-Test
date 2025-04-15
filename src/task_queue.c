#include "../include/task_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>  // Add this for strerror()

int task_queue_init(TaskQueue *queue) {
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    
    if (pthread_mutex_init(&queue->lock, NULL) != 0) {
        fprintf(stderr, "Mutex init failed: %s\n", strerror(errno));
        return -1;
    }
    
    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
        fprintf(stderr, "Cond var init failed: %s\n", strerror(errno));
        pthread_mutex_destroy(&queue->lock);
        return -1;
    }
    
    if (pthread_cond_init(&queue->not_full, NULL) != 0) {
        fprintf(stderr, "Cond var init failed: %s\n", strerror(errno));
        pthread_mutex_destroy(&queue->lock);
        pthread_cond_destroy(&queue->not_empty);
        return -1;
    }
    
    return 0;
}

// ... rest of task_queue.c remains the same ...
void task_queue_destroy(TaskQueue *queue) {
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
}

bool task_queue_push(TaskQueue *queue, const Task *task) {
    pthread_mutex_lock(&queue->lock);
    
    while (queue->count == MAX_QUEUE_SIZE) {
        pthread_cond_wait(&queue->not_full, &queue->lock);
    }
    
    queue->tasks[queue->tail] = *task;
    queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;
    queue->count++;
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->lock);
    return true;
}

bool task_queue_pop(TaskQueue *queue, Task *out_task) {
    pthread_mutex_lock(&queue->lock);
    
    while (queue->count == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->lock);
    }
    
    *out_task = queue->tasks[queue->head];
    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    queue->count--;
    
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);
    return true;
}

bool task_queue_is_empty(TaskQueue *queue) {
    pthread_mutex_lock(&queue->lock);
    bool empty = (queue->count == 0);
    pthread_mutex_unlock(&queue->lock);
    return empty;
}

bool task_queue_is_full(TaskQueue *queue) {
    pthread_mutex_lock(&queue->lock);
    bool full = (queue->count == MAX_QUEUE_SIZE);
    pthread_mutex_unlock(&queue->lock);
    return full;
}