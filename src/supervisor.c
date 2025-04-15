#define _POSIX_C_SOURCE 200809L  // For modern POSIX features
#include "../include/supervisor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define MAX_PROCESSES 100

typedef struct {
    pid_t pid;
    bool active;
} TrackedProcess;

static TrackedProcess processes[MAX_PROCESSES];
static pthread_mutex_t processes_mutex = PTHREAD_MUTEX_INITIALIZER;

static void sigchld_handler(int sig) {
    (void)sig;  // Explicitly mark unused parameter
    int status;
    pid_t pid;
    int saved_errno = errno;  // Save errno
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        pthread_mutex_lock(&processes_mutex);
        
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processes[i].pid == pid && processes[i].active) {
                processes[i].active = false;
                
                if (WIFSIGNALED(status)) {
                    fprintf(stderr, "[Supervisor] Process %d terminated by signal %d\n",
                            pid, WTERMSIG(status));
                } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                    fprintf(stderr, "[Supervisor] Process %d exited with status %d\n",
                            pid, WEXITSTATUS(status));
                }
                break;
            }
        }
        
        pthread_mutex_unlock(&processes_mutex);
    }
    errno = saved_errno;  // Restore errno
}

int supervisor_init() {
    // Initialize process tracking array
    memset(processes, 0, sizeof(processes));
    
    // Set up SIGCHLD handler
    struct sigaction sa = {
        .sa_handler = sigchld_handler,
        .sa_flags = SA_RESTART | SA_NOCLDSTOP
    };
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "[Supervisor] Failed to set up signal handler: %s\n",
                strerror(errno));
        return -1;
    }
    return 0;
}

int supervisor_register_pid(pid_t pid) {
    pthread_mutex_lock(&processes_mutex);
    
    int registered = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!processes[i].active) {
            processes[i].pid = pid;
            processes[i].active = true;
            registered = 0;
            break;
        }
    }
    
    pthread_mutex_unlock(&processes_mutex);
    
    if (registered == -1) {
        fprintf(stderr, "[Supervisor] Cannot register PID %d: process table full\n", pid);
    }
    return registered;
}

bool supervisor_is_process_alive(pid_t pid) {
    pthread_mutex_lock(&processes_mutex);
    
    bool alive = false;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid && processes[i].active) {
            alive = true;
            break;
        }
    }
    
    pthread_mutex_unlock(&processes_mutex);
    return alive;
}

void supervisor_cleanup() {
    pthread_mutex_lock(&processes_mutex);
    
    // Send SIGTERM to all active processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].active) {
            kill(processes[i].pid, SIGTERM);
            processes[i].active = false;
        }
    }
    
    pthread_mutex_unlock(&processes_mutex);
    pthread_mutex_destroy(&processes_mutex);
}