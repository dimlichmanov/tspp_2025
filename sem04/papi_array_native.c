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
    int retval, EventSet = PAPI_NULL;
    int EventCode;
    long long cm;

    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        printf("PAPI library init error!\n");
        exit(1);
    }

    handle_error(PAPI_event_name_to_code("ix86arch::UNHALTED_CORE_CYCLES", &EventCode), "event_name_to_code");
    handle_error(PAPI_query_event(EventCode), "query_event");

    handle_error(PAPI_create_eventset(&EventSet), "create_event_set");

    handle_error(PAPI_add_event(EventSet, EventCode), "add_event");

    unsigned int native = 0x0;
    PAPI_event_info_t info;

    handle_error(PAPI_get_event_info(EventCode, &info), "get_event_info");
    printf("\n%d, %s, count: %s\n", info.event_code, info.symbol, info.short_descr);

    double *array = NULL, sum;
    int n = 1024 * 1024;

    array = calloc(n, sizeof(array[0]));
    if (!array) {
        fputs("memory allocation error!\n", stderr);
        return 1;
    }

    srand(323);

    handle_error(PAPI_start(EventSet), "papi_start");
    
    generate_array(array, n);
    sum = array_sum(array, n);

    handle_error(PAPI_stop(EventSet, &cm), "papi_stop");
    
    printf("sum = %f\n", sum);
    printf("cm = %lld\n", cm);

    free(array);
    
    return 0;
}