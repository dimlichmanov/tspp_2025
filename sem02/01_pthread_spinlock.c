#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int iters = 1000000;

pthread_spinlock_t lock;
long long counter = 0;

//TODO correct 
void* worker(void* arg) {
    for (int i = 0; i < iters; ++i) {
        pthread_spin_lock(&lock);
        counter++;
        pthread_spin_unlock(&lock);
    }
    return NULL;
}

int main(void) {
    int THREAD_NUM = 4;

    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);

    pthread_t threads[4];
    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_spin_destroy(&lock);

    printf("Counter = %lld\n", counter);

    pthread_exit(NULL);
}
