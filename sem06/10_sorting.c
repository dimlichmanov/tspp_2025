#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRAIN 2000 

void swap_(int *a, int *b) { int t = *a; *a = *b; *b = t; }

int partition(int *a, int lo, int hi) {
    int pivot = a[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; ++j) {
        if (a[j] <= pivot) { ++i; swap_(&a[i], &a[j]); }
    }
    swap_(&a[i + 1], &a[hi]);
    return i + 1;
}

void quicksort(int *a, int lo, int hi) {
    if (lo >= hi) return;
    int p = partition(a, lo, hi);

    #pragma omp taskgroup
    {
        #pragma omp task shared(a) firstprivate(lo, p) if (p - lo > GRAIN)
        quicksort(a, lo, p - 1);

        #pragma omp task shared(a) firstprivate(hi, p) if (hi - p > GRAIN)
        quicksort(a, p + 1, hi);
    }
}

int main(int argc, char **argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 1 << 24;
    int *a = (int*)malloc(n * sizeof(int));
    if (!a) return 1;

    srand(323);
    for (int i = 0; i < n; ++i) a[i] = rand();

    double t0 = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        quicksort(a, 0, n - 1);
    }

    double t1 = omp_get_wtime();
    for (int i = 1; i < n; ++i) {
        if (a[i-1] > a[i]) { fprintf(stderr, "Not sorted at %d\n", i); break; }
    }

    printf("Sorted %d elements in %.3f s with %d threads\n",
           n, t1 - t0, omp_get_max_threads());
    free(a);
    return 0;
}