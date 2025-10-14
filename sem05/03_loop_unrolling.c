// Example is taken from A.Gorobets lectures 
//gcc-14 02_loop_unrolling_base.c -fopenmp
#include "omp.h"
#include "stdio.h"

int main () {
    int N = 330000123;
    double a = 1.0, result = 0.0, t;

    t = omp_get_wtime();

    result = 0.0;
    double result_temp[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    for(int i=0; i<N/8; ++i)  {
        result_temp[0] += a;
        result_temp[1] += a;
        result_temp[2] += a;
        result_temp[3] += a; 
        result_temp[4] += a;
        result_temp[5] += a;
        result_temp[6] += a;
        result_temp[7] += a;
    }

    for (int i = 0; i < N%8; i++) {
        result += a;
    }

    result += result_temp[0] + result_temp[1] + result_temp[2] + result_temp[3];
    result += result_temp[4] + result_temp[5] + result_temp[6] + result_temp[7];
    
    t = omp_get_wtime() - t;
    
    printf("ADD X8 res: %15.12E time: %5.3f s GFLOPS: %5.3f\n", result, t, N/(t*1E9));
}
