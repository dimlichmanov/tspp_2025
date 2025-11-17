#include "mpi.h"
#include <stdio.h>

/* Создаём вектор-тип, где базовый тип - полоски. */
int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Datatype type_final, type_cont;
    int buffer[24];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Type_contiguous(3, MPI_INT, &type_cont);
    MPI_Type_commit(&type_cont);

    MPI_Type_vector(3, 2, 3, type_cont, &type_final);
    MPI_Type_commit(&type_final);

    if (rank == 0)
    {
        for (i=0; i<24; i++) {
            buffer[i] = i;
        }
        MPI_Send(buffer, 1, type_final, 1, 123, MPI_COMM_WORLD);
    }

    if (rank == 1)
    {
        for (i=0; i < 24; i++) {
            buffer[i] = -1;
        }

        MPI_Recv(buffer, 1, type_final, 0, 123, MPI_COMM_WORLD, &status);
        
        for (i=0; i<24; i++) {
            printf("buffer[%d] = %d\n", i, buffer[i]);
        }
    }

    MPI_Finalize();
    return 0;
}