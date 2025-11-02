#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


int main(int argc, char** argv) {

    int comm_size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int root_rank = 1; 
    int reduction_result = 0;

    MPI_Reduce(&rank, &reduction_result, 1, MPI_INT, MPI_SUM, root_rank, MPI_COMM_WORLD);

    printf("%d/%d: The sum of all ranks is %d\n", rank, comm_size, reduction_result);

    MPI_Finalize();
    return 0;
}