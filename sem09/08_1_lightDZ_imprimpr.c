#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <assert.h>
#include <stdbool.h>

// Написать параллельную программу на MPI, которая заменяет элементы вектора
// следующим образом: каждый элемент заменяется на среднее значение его соседних
// элементов, т.е. a[i] = (a[i-1] + a[i+1])/2.  
// Преобразования проводить без заведения дополнительного вектора, используя только вспомогательные переменные.
// Значения первого и последнего элементов не меняются.

// Элементы вектора распределяются по процессам равномерно.

// Требования: каждый процесс должен выполнять преобразование своего блока, для решения задачи нельзя использовать дополнительный массив.

// Начальное значение элементов задать случайными числами. Размер блока вектора задается в командной строке. Нулевой процесс рассылает это значение всем процессам.


void print_in_rank_order(const float* my_array, int length, int rank, int comm_size)
{
    MPI_Barrier(MPI_COMM_WORLD);
    for (int r = 0; r < comm_size; ++r) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (r == rank) {
            printf("[rank %d]", rank);
            for (int i = 0; i < length; i++) {
                printf("%f ", my_array[i]);
            }
            printf("\n\n");
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

void swap(float* a, float* b) {
    float c = *a;
    *a = *b;
    *b = c;
}

#define MY_DUMMY_TAG 1

int main(int argc, char** argv) {
    int comm_size, rank;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    int N = 0; 

    if (rank == 0) {
        N = 20;
        if (argc > 1) N = atoi(argv[1]);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    assert(N >= comm_size);

    int local_n = N / comm_size + ((rank < N % comm_size) ? 1 : 0);

    assert(local_n > 0);

    float* my_part = (float*) malloc(local_n * sizeof(float));

    srand(rank * 32 + 323 * comm_size);

    for (int i = 0; i < local_n; i++) {
        my_part[i] = (float)(rand() % 100);
    }   

    print_in_rank_order(my_part, local_n, rank, comm_size);

    /* Тут я написал, как посчитать left и right соседей через одномерную топологию. 
    Вряд ли тут это удобнее, но как пример */
    
    MPI_Comm comm_1d;
    int periods[1] = {0}; // периодичность не нужна, в отличие от задачи с блоками и максимальным элементом
    int dims[1] = {comm_size};
    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, 0, &comm_1d);
    int left_neighbour;
    int right_neighbour;

    MPI_Cart_shift(comm_1d, 0, 1, &left_neighbour, &right_neighbour);

    /* Ошибка на семинаре была в том, что вместо float я написал int */
    float left_elem;
    float right_elem;

    MPI_Sendrecv(&my_part[0], 1, MPI_FLOAT, left_neighbour, MY_DUMMY_TAG, &right_elem, 1, MPI_FLOAT, right_neighbour, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Sendrecv(&my_part[local_n - 1], 1, MPI_FLOAT, right_neighbour, MY_DUMMY_TAG, &left_elem, 1, MPI_FLOAT, left_neighbour, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    float temp_value_1; 
    float temp_value_2; 
    
    temp_value_1 = my_part[0];

    if (rank != 0) {
        my_part[0] = (left_elem + my_part[1]) / 2;
    } 

    for (int i = 1; i < local_n - 1; i++) {
        temp_value_2 = my_part[i];
        my_part[i] = (temp_value_1 + my_part[i + 1]) / 2;
        swap(&temp_value_1, &temp_value_2);
    }   

    if (rank != comm_size - 1) {
        my_part[local_n - 1] = (right_elem + temp_value_1) / 2;
    }
    
    print_in_rank_order(my_part, local_n, rank, comm_size);
    
    MPI_Finalize();
    free(my_part);

}