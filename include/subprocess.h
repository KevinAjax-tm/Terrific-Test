#pragma once
#include "common.h"

typedef struct {
    pid_t pid;
    int timeout_sec;
    char **argv;
} Subprocess;

int run_subprocess(Subprocess *proc);