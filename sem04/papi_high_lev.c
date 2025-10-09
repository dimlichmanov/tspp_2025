#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <time.h>

void handle_error(int retval, const char *str)
{
    if (retval != PAPI_OK) {
        fprintf(stderr,
                "%s: PAPI error %d: %s\n",
                str, retval, PAPI_strerror(retval));
        exit(1);
    }
}

void generate_array(double *a, int size)
{
    int i;
    for (i = 0; i < size; ++i) {
        a[i] = (double) rand();
    }
}

double array_sum(double *a, int size)
{
    int i;
    double res = 0.0;
    for (i = 0; i < size; ++i) {
        res += a[i];
    }
    return res;
}

int main(int argc, char **argv)
{
    double *array = NULL, sum;
    int n = 1024, retval;

    float rtime, ptime, mflops;
    long long flpops;

    array = calloc(n, sizeof(array[0]));
    if (!array) {
        fputs("memory allocation error!\n", stderr);
        return 1;
    }
    srand(323);

    handle_error(PAPI_flops_rate(PAPI_DP_OPS, &rtime, &ptime,
                             &flpops, &mflops), "papi_flops_rate1");
    
    generate_array(array, n);
    sum = array_sum(array, n);

    handle_error(PAPI_flops_rate(PAPI_DP_OPS, &rtime, &ptime,
                             &flpops, &mflops), "flops_rate2");

    printf("sum = %f\n", sum);
    printf("real time = %f\n"
           "process time = %f\n"
           "floating point operations = %lld\n"
           "mflops = %f\n",
           rtime, ptime, flpops, mflops);


    free(array);
    
    return 0;
}