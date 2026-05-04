#include <stdio.h>
#include <stdlib.h>

#define NUM_PROCS 15
#define NUM_RES 15

static int waiting[NUM_PROCS][NUM_RES];
static int held_by[NUM_RES];

void init_matrix(void) {
    for (int p = 0; p < NUM_PROCS; p++) {
        for (int r = 0; r < NUM_RES; r++) {
            waiting[p][r] = 0;
        }
    }
    for (int r = 0; r < NUM_RES; r++) {
        held_by[r] = -1;
    }
}

int search_process(int visited[NUM_PROCS], int start, int current);

int search_resource(int visited[NUM_PROCS], int start, int current) {
    int holder = held_by[current];
    if (holder == -1) {
        return 0;
    }
    if (holder == start) {
        printf("RESOURCE %d -> PROCESS %d\n", current, holder);
        return 1;
    }
    if (visited[holder]) {
        return 0;
    }
    visited[holder] = 1;
    if (search_process(visited, start, holder)) {
        printf("RESOURCE %d -> PROCESS %d\n", current, holder);
        return 1;
    }
    return 0;
}

int search_process(int visited[NUM_PROCS], int start, int current) {
    for (int r = 0; r < NUM_RES; r++) {
        if (waiting[current][r]) {
            if (search_resource(visited, start, r)) {
                printf("PROCESS %d -> RESOURCE %d\n", current, r);
                return 1;
            }
        }
    }
    return 0;
}

int start_search(int start_process) {
    int visited[NUM_PROCS] = {0};
    visited[start_process] = 1;
    return search_process(visited, start_process, start_process);
}

void acquire(int process, int resource) {
    if (held_by[resource] == -1) {
        held_by[resource] = process;
        return;
    }
    waiting[process][resource] = 1;
    if (start_search(process)) {
        exit(-1);
    }
}

int release(int process, int resource) {
    if (held_by[resource] != process) {
        return 0;
    }
    held_by[resource] = -1;
    for (int p = 0; p < NUM_PROCS; p++) {
        if (waiting[p][resource]) {
            waiting[p][resource] = 0;
            held_by[resource] = p;
            if (start_search(p)) {
                exit(-1);
            }
            return p;
        }
    }
    return -1;
}

int main(void) {
    init_matrix();
    int process, resource;
    char action;
    while (scanf(" %d %c %d", &process, &action, &resource) == 3) {
        if (process < 0 || process >= NUM_PROCS) continue;
        if (resource < 0 || resource >= NUM_RES) continue;
        if (action == 'a') {
            acquire(process, resource);
        } else if (action == 'r') {
            release(process, resource);
        }
    }
    return 0;
}
