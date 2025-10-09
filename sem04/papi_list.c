#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <unistd.h>
#include <stdint.h>

void handle_error(int retval, const char *str)
{
    if (retval != PAPI_OK) {
        fprintf(stderr,
                "%s: PAPI error %d: %s\n",
                str, retval, PAPI_strerror(retval));
        exit(1);
    }
}

struct item
{
    double val;
    struct item *next;
};

struct item * gen_list(int n)
{
    struct item *res = NULL;
    int i;
    for (i = 0; i < n; ++i) {
        struct item *new_el;

        new_el = (struct item *)calloc(1, sizeof(*new_el));
        if (!new_el) {
            fputs("memory allocation error\n", stderr);
            return NULL;
        }
        new_el->next = res;
        new_el->val = (double) rand();
        res = new_el;
    }
    return res;
}

void calculate_stride(const struct item *head,
                      unsigned long long *sum_bytes)
{
    *sum_bytes = 0;

    for (const struct item *p = head; p && p->next; p = p->next) {
        uintptr_t a = (uintptr_t)p;
        uintptr_t b = (uintptr_t)p->next;
        unsigned long long d = (a > b) ? (a - b) : (b - a); 
        *sum_bytes += d;
    }
}

void list_free(struct item *l)
{
    struct item *p = l;
    while(p) {
        struct item *tmp = p;
        p = p->next;
        free(tmp);
    }
}



double list_sum(struct item *l)
{
    struct item *p = l;
    double sum = 0.0;

    while(p) {
        sum += p->val;
        p = p->next;
    }
    return sum;
}


void gen_array(double *a, int size)
{
    int i;
    for (i = 0; i < size; ++i) {
        a[i] = (double) rand();
    }
}

double array_sum(double *a, int size)
{
    double res = 0.0;
    int i;
    for (i = 0; i < size; ++i) {
        res += a[i];
    }
    return res;
}

int main(int argc, char **argv)
{
    int retval;
    int event_set = PAPI_NULL;
    long long cm1, cm2;
    
    int n = 1024 * 1024;

    printf("Size of item in a list: %ld\n", sizeof(struct item));
    printf("Size of double: %ld\n", sizeof(double));

    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        fputs("failed to initialize PAPI\n", stderr);
        return 1;
    }

    handle_error(PAPI_create_eventset(&event_set), "create_event_set");

    handle_error(PAPI_add_event(event_set, PAPI_L1_DCM), "add_event");

    double *a = NULL, sum;

    a = (double*)calloc(n, sizeof(a[0]));


    srand(323);
    gen_array(a, n);

    handle_error(PAPI_start(event_set), "papi_start"); 
    sum = array_sum(a, n);
    handle_error(PAPI_stop(event_set, &cm1), "papi_stop");

    printf("For array: sum = %f\n", sum);
    free(a);

    struct item *l = NULL;
    
    srand(323);
    l = gen_list(n);
        long long int stride = 0;
    calculate_stride(l, &stride);
    printf("Average stride: %lf\n", (double)stride/n);

    handle_error(PAPI_start(event_set), "papi_start");
    sum = list_sum(l);
    handle_error(PAPI_stop(event_set, &cm2), "papi_stop");

    printf("For list: sum = %f\n", sum);
    list_free(l);

    printf("cache misses for array = %lld\n"
           "cache misses for list = %lld\n",
           cm1, cm2);
    
    return 0;
}


















