#include <pthread.h>
#include <stdio.h>
#include <unistd.h> 
#include <stdbool.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv  = PTHREAD_COND_INITIALIZER;
bool ready = false;

void* worker(void* param) {
    sleep(1);

    pthread_mutex_lock(&mtx);
    ready = true;
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mtx);
    return NULL;
}

int main(void){
    pthread_t th;
    pthread_create(&th, NULL, worker, NULL);

    pthread_mutex_lock(&mtx);

    while (!ready) {
        pthread_cond_wait(&cv, &mtx);
    }

    pthread_mutex_unlock(&mtx);

    printf("Ready = %d ", ready);
    pthread_join(th, NULL);
    return 0;
}


