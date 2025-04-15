#include "../include/common.h"
#include "../include/thread_pool.h"
#include "../include/ui.h"
#include "../include/ipc.h"
#include "../include/supervisor.h"
#include "../include/task_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static volatile sig_atomic_t shutdown_requested = 0;

void handle_sigint(int sig) {
    (void)sig;
    shutdown_requested = 1;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;  // Mark unused parameters
    
    // Signal handling
    struct sigaction sa = {0};
    sa.sa_handler = handle_sigint;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    // Initialize components
    SharedResults* shmem = ipc_init();
    TaskQueue queue;
    if (task_queue_init(&queue) != 0) {
        fprintf(stderr, "Task queue init failed\n");
        return EXIT_FAILURE;
    }

    if (supervisor_init() != 0) {
        fprintf(stderr, "Supervisor init failed\n");
        return EXIT_FAILURE;
    }

    ThreadPool pool;
    if (thread_pool_init(&pool, 8, &queue, shmem) != 0) {
        fprintf(stderr, "Thread pool init failed\n");
        return EXIT_FAILURE;
    }

    if (init_dashboard() == 0) {
        fprintf(stderr, "UI init failed\n");
        return EXIT_FAILURE;
    }

    while (!shutdown_requested) {
        update_ui(&pool, shmem);
        usleep(100000);
    }

    // Cleanup
    thread_pool_shutdown(&pool);
    task_queue_destroy(&queue);
    ipc_cleanup(shmem);
    return EXIT_SUCCESS;
}