#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int root_rank = 2;

    int x = (comm_size - 1 - rank) * 10;

    int* all = (int*)malloc(comm_size * sizeof(int));

    MPI_Allgather(&x, 1, MPI_INT, all, 1, MPI_INT, MPI_COMM_WORLD);

    //if (rank == root_rank) {
        printf("Gathered on rank: %d\n", rank);
        for (int i = 0; i < comm_size; ++i) printf("%d ", all[i]);
        printf("\n");
        free(all);
    //}

    MPI_Finalize();
    return 0;
}