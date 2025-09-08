#define _GNU_SOURCE
#include <sched.h>  
#include <sys/mman.h>   
#include <sys/syscall.h> 
#include <linux/futex.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Shared { volatile long a; volatile long b; };

static int futex_wait(volatile int *addr, int expected) {
    return syscall(SYS_futex, addr, FUTEX_WAIT, expected, NULL, NULL, 0);
}

static int child_func(void *arg) {
    struct Shared *sh = (struct Shared*)arg;
    for (int i = 0; i < 1e6; ++i) sh->a++;
    return 0; 
}

int main(void) {
    const size_t stack_size = 1 << 20;

    void* stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

    struct Shared *sh = (struct Shared*)malloc(sizeof(struct Shared));
    
    sh->a = 90;
    sh->b = 42;

    static volatile int child_tid = 0;

    int flags =
        CLONE_VM         |   
        CLONE_FS         |  
        CLONE_FILES      |   
        CLONE_SIGHAND    |  
        CLONE_THREAD     |   
        CLONE_SYSVSEM    |   
        CLONE_PARENT_SETTID |
        CLONE_CHILD_CLEARTID | 
        CLONE_CHILD_SETTID;

    pid_t ptid_storage;
    pid_t child = clone(child_func, stack + stack_size, flags, sh,
                        &ptid_storage, 
                        NULL,    
                        (int *)&child_tid);

    while (ptid_storage != child_tid) {
        __asm__ __volatile__("pause");
    }

    for (int i = 0; i < 10; ++i) sh->b++;

    while (child_tid != 0) {
        if (futex_wait(&child_tid, ptid_storage) == -1) {
            perror("futex_wait"); break;
        }
    }

    printf("a = %ld, b = %ld (ptid=%d)\n", sh->a, sh->b, (int)ptid_storage);

    munmap(stack, stack_size);
    free(sh);
    return 0;
}