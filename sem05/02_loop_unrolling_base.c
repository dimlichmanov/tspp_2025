//gcc-14 02_loop_unrolling_base.c -fopenmp

#include <stdio.h>
#include "omp.h"

int main () {
    int N=330000123;

    double a = 1.0, result = 0.0, t;
    
    t = omp_get_wtime();
    
    result = 0.0;
    for(int i=0; i<N; ++i) {
        result += a;
    }
    
    t = omp_get_wtime() - t;
    
    printf("ADD X1 res: %15.12E time: %5.3f s GFLOPS: %5.3f\n", result, t, N/(t*1E9));
}