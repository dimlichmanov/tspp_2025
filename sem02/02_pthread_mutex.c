#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/time.h>

struct Shared {
    int a; 
    long long h;
    long long k;
    long long j;
    long long d;
    long long p;
    int b;
};

void* func(void* param) {
    struct Shared* ptr = (struct Shared*)(param);
    for (int i = 0; i< 10000000; i++) {
         ptr->a++;
    }
}


int main() {
    pthread_t thread1;

    struct timeval begin, end;

    struct Shared* ptr = (struct Shared*)malloc(sizeof(struct Shared));
    ptr->a = 0;
    ptr->b = 0;

    gettimeofday(&begin, NULL);
    pthread_create(&thread1, NULL, func, ptr);

    for (int i = 0; i< 10000000; i++) {
        ptr->b++;
    }

    pthread_join(thread1, NULL);

    gettimeofday(&end, NULL);

    printf("%d\n", ptr->a + ptr->b);

    printf("%lf\n", end.tv_sec - begin.tv_sec + end.tv_usec/1e6 - begin.tv_usec/1e6);
}



    