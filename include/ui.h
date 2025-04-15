#pragma once
#include "thread_pool.h"
#include "ipc.h"

int init_dashboard(void);  // Note: void in parameter list
void update_ui(ThreadPool *pool, SharedResults *results);