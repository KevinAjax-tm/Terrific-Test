#include "../include/ui.h"
#include <ncurses.h>

int init_dashboard(void) {
    if (initscr() == NULL) return 0;
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    return 1;
}

void update_ui(ThreadPool *pool, SharedResults *results) {
    (void)results;  // Mark unused parameter
    clear();
    printw("=== Distask Live Dashboard ===\n\n");
    printw("-- Thread Pool Status --\n");
    for (int i = 0; i < pool->max_threads; i++) {
        printw("Thread %d: %s\n", 
              i, pool->workers[i].active ? "BUSY" : "IDLE");
    }
    refresh();
}

void cleanup_dashboard(void) {
    endwin();
}