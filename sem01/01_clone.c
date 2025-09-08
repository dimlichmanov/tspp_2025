#define _GNU_SOURCE  
#include "stdlib.h"
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>
#include <sched.h>


struct Shared {
    volatile int a; volatile int b;
};

int child_func(void* params) {
    struct Shared* ptr = (struct Shared*)(params);
    for (int i = 0; i < 1000000; i++) {
        ptr->a++;
    }
}

int main() 
{
    size_t stacksize = 1 << 20;

    struct Shared* ptr_my = (struct Shared*)malloc(sizeof(struct Shared));
    
    ptr_my->a = 20;
    ptr_my->b = 40;

    void* stack = mmap(NULL, stacksize, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED | MAP_STACK, -1, 0);
    
    for (int i = 0; i < 1000; i++) {
        ptr_my->b++;
    }

    pid_t tid = clone(child_func, stack + stacksize, CLONE_VM | SIGCHLD, (void*)ptr_my);

    int status;
    waitpid(tid, &status, 0);


    printf("%d -- %d\n", ptr_my->a, ptr_my->b);
}