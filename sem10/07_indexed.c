#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

/* Индексный тип для пересылки верхнетреугольной матрицы - см.пример */
int main() {

    int a[8][8];
    MPI_Datatype triang;
    MPI_Aint lb, sz;

    int myrank, ierr;
    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Type_get_extent(MPI_INT, &lb, &sz);

    int dspls[8];
    int block_lengths[8];

    for (int i = 0; i < 8; i++) {
        dspls[i] = 7 * (i + 1); // помним, что раз сдвиг нулевого блока = 7, 
                                // то мы должны начать отправку с нулевого элемента матрицы
                                // если displs[i] = 7i, то отправку начинаем с &a[0][7]
        block_lengths[i] = (i + 1);
    }

    if (myrank == 0) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                a[i][j] = i*8 + j;
            }
        }
    } else {
       for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                a[i][j] = -1;
            }
        } 
    }

    MPI_Type_indexed(8, block_lengths, dspls, MPI_INT, &triang);
    MPI_Type_commit(&triang);

    if (myrank == 0) {
        MPI_Type_get_extent(triang, &lb, &sz);
        printf("LB: %ld, extent: %ld\n", lb, sz);

        MPI_Send(&a[0][0], 1, triang, 1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&a[0][0], 1, triang, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) {
                printf("%d ", a[i][j]);
            }
            printf("\n");
        }
        
    }
    
    MPI_Type_free(&triang);
    MPI_Finalize();
}