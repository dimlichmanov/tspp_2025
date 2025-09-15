#define _GNU_SOURCE
#include "sched.h"

#include "pthread.h"
#include "stdlib.h"
#include "stdio.h"

void* my_func(void* param) {
    int p = 0;
    while (p == 0) {
        for (int i = 0; i < 1>>20; i++) {
            p++;
        }
        if (p == 1>>20) {
            p = 0;
        }
    }
}

int main() {
    pthread_t thread1;
    pthread_t thread2;

    cpu_set_t my_set;

    pthread_attr_t my_attr;

    pthread_attr_init(&my_attr);
    
    CPU_ZERO(&my_set);
    
    CPU_SET(2, &my_set);
    CPU_SET(0, &my_set);
    
    pthread_attr_setaffinity_np(&my_attr, sizeof(cpu_set_t), &my_set);

    
    pthread_create(&thread1, &my_attr, my_func, NULL);
    pthread_create(&thread2, &my_attr, my_func, NULL);

    // pthread_setaffinity_np(thread1, sizeof(my_set), &my_set);
    // pthread_setaffinity_np(thread2, sizeof(my_set), &my_set);


    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

}