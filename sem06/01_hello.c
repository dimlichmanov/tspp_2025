#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {

    int b = 0;

    #pragma omp parallel for 
    for (int i = 0; i < 16; i++){
            printf("Hello from thread:  %d\n", b);
    }
    return 0;
}