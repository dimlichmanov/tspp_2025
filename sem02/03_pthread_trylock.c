#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>

typedef struct {
    pthread_mutex_t mtx;
    int balance;
} Account;

Account A = { PTHREAD_MUTEX_INITIALIZER, 100000 };
Account B = { PTHREAD_MUTEX_INITIALIZER, 100000 };

void transfer(Account* from, Account* to, int amount) {
    
    for (;;) {
        pthread_mutex_lock(&from->mtx);

        int rc = pthread_mutex_trylock(&to->mtx);

        if (rc == 0) {
            from->balance -= 1;
            to ->balance++;

            pthread_mutex_unlock(&from->mtx);
            pthread_mutex_unlock(&to->mtx);

            return;
        }

        if (rc == EBUSY) {
            pthread_mutex_unlock(&from->mtx);
            continue;
        }



    }
}

void* worker_ab(void* arg) {
    for (int i = 0; i < 100000; ++i) transfer(&A, &B, 1);
    return NULL;
}

void* worker_ba(void* arg) {
    for (int i = 0; i < 100000; ++i) transfer(&B, &A, 1);
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, worker_ab, NULL);
    pthread_create(&t2, NULL, worker_ba, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("A.balance=%d  B.balance=%d  total=%d\n",
           A.balance, B.balance, A.balance + B.balance);
    return 0;
}