#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>

struct Shared {
    int begin;
    int end;

    double my_res;
    double segm_length;
};

void* func(void* param) {

    struct Shared* ptr = (struct Shared*)(param);
    for (int i = ptr->begin; i< ptr->end; i++) {
        double x = i * ptr->segm_length;
        ptr->my_res += 4.0 / (1 + x * x); 
    }
}

int main() {
    int N_threads = 4;
    pthread_t threads[N_threads];

    int n_segments = 1000000;

    struct Shared* ptr = (struct Shared*)malloc(N_threads* sizeof(struct Shared));

    for (int i = 0; i < 4; i++) {
        ptr[i].begin = i * (n_segments/4);
        ptr[i].end = (i+1) * (n_segments/4);

        ptr[i].my_res = 0.0;
        ptr[i].segm_length = 1.0/n_segments;
    }

    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, func, &ptr[i]);
    }
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    double global_sum = 0.0;

    for (int i = 0; i < 4; i++) {
        global_sum += ptr[i].my_res;
    }

    printf("%lf\n", global_sum);
}




// ptr[i].begin = i * (n_segments/4);
//         ptr[i].end = (i + 1) * (n_segments/4);
//         ptr[i].my_res = 0.0;
//         ptr[i].segm_length = 1.0/n_segments;