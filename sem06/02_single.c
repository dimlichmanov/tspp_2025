#include <stdio.h>
#include <omp.h>
int main (void)
{
    omp_set_max_active_levels(2);
    omp_set_dynamic(0);

    //printf ("Before: num_thds=%d\n", omp_get_num_threads());

    //omp_set_num_threads(4);

    int i = 0;

#pragma omp parallel num_threads(2)
{
    #pragma omp single
    {
        printf ("Inner: num_threads=%d\n", omp_get_num_threads());
    }

    #pragma omp parallel num_threads(2)
    {
        #pragma omp single
        {
            printf ("Inner Inner : num_threads=%d\n", omp_get_num_threads());
        }
    }
}
    return 0;
}