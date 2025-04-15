#include "../include/subprocess.h"
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>  
#include <signal.h>  // For SIGKILL, SIGCHLD, etc.// For EXIT_FAILURE/EXIT_SUCCESS
int run_subprocess(Subprocess *proc) {
    int status;
    pid_t child_pid = fork();
    
    if (child_pid == 0) {
        execvp(proc->argv[0], proc->argv);
        _exit(EXIT_FAILURE);  // Use _exit() in child
    } 
    else {
        time_t start = time(NULL);
        while (1) {
            int ret = waitpid(child_pid, &status, WNOHANG);
            if (ret == child_pid) break;
            
            if (time(NULL) - start > proc->timeout_sec) {
                kill(child_pid, SIGKILL);
                return -1;
            }
            usleep(100000);
        }
        return WEXITSTATUS(status);
    }
}