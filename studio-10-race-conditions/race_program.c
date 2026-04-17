#include <stdio.h>
#include <pthread.h>

int race = 0;

void *adder(void *arg) {
    for (int i = 0; i < 2000000; i++) {
        race = race + 1;
    }
    return NULL;
}

void *subtractor(void *arg) {
    for (int i = 0; i < 2000000; i++) {
        race = race - 1;
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, adder, NULL);
    pthread_create(&t2, NULL, subtractor, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("race = %d\n", race);
    return 0;
}
