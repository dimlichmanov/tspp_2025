#include "pthread.h"


struct barrier_t {
    int max_threads;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int cnt; //Изначально = max_threads
};

void* barrier_wait(struct barrier_t* bar){
    pthread_mutex_lock(&bar->mutex);
    bar->cnt--;

    if (bar->cnt == 0) {
        pthread_cond_broadcast(&bar->cond);
    } else {
        while (bar->cnt != 0) {
            pthread_cond_wait(&bar->cond, &bar->mutex);
        }
    }


    pthread_mutex_unlock(&bar->mutex);


}