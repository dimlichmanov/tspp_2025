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
    pthread_t thread1;

    struct Shared* ptr = (struct Shared*)malloc(sizeof(struct Shared));
    pthread_create(&thread1, NULL, func, NULL);

    for (int i = 0; i< 1000000; i++) {
        ptr->b++;
    }

    pthread_join(thread1, NULL);

    printf("%d\n", ptr->a + ptr->b);

}
