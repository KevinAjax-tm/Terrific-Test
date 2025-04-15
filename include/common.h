#pragma once
#include <stdbool.h>
#include <pthread.h>

typedef struct {
    int task_id;
    char **argv;  // NULL-terminated command array
    int timeout_sec;
} Task;

typedef struct {
    int task_id;
    int exit_code;
    double exec_time;
} TaskResult;