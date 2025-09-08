#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>


void* func(void* param) {
    struct Shared* ptr = (struct Shared*)(param);
    for (int i = 0; i< 1000000; i++) {
         ptr->a++;
    }
}

struct Shared {
    int a; int b;
};


int main() {
    int N_threads = 4;
    pthread_t threads[N_threads];

    struct Shared* ptr = (struct Shared*)malloc(N_threads* sizeof(struct Shared));

    for (int i = 0; i < 4; i++) {
        ptr->a = i;
        ptr->b = 2*i;

    }

    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, func, &ptr[i]);
    }
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("%d\n", d);
}