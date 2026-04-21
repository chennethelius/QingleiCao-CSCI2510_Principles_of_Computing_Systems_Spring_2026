#include <stdio.h>
#include <pthread.h>

#define ITERATIONS 20000000

int race = 0;

void *adder(void *arg) {
    pthread_mutex_t *m = (pthread_mutex_t *)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(m);
        race = race + 1;
        pthread_mutex_unlock(m);
    }
    return NULL;
}

void *subtractor(void *arg) {
    pthread_mutex_t *m = (pthread_mutex_t *)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(m);
        race = race - 1;
        pthread_mutex_unlock(m);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_create(&t1, NULL, adder, &m);
    pthread_create(&t2, NULL, subtractor, &m);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("race = %d\n", race);
    return 0;
}
