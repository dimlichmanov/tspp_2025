#include <stdio.h>
#include <stdbool.h>
#include "omp.h"

/* Это пример на if(), чтобы поток сразу же выполнил поставленный им же task */
int main() {
    #pragma omp parallel
    {
        int orig_id = omp_get_thread_num();
        #pragma omp task if(false) 
        {
            int my_id = omp_get_thread_num();
            printf("Hello, i am %d, originally %d\n", my_id, orig_id);
        }
    }

    return 0;
}