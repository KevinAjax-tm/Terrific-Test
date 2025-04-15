#pragma once
#include <stdbool.h>
#include <sys/types.h>

#define MAX_PROCESSES 100

int supervisor_init(void);
int supervisor_register_pid(pid_t pid);
bool supervisor_is_process_alive(pid_t pid);
void supervisor_cleanup(void);