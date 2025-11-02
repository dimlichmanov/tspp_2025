#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int n = 1000;
    int comm_size, rank;
    int *x = calloc(n, sizeof(x[0]));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);


    if (rank == 0) {
        x[0] = 100500;
        x[323] = 42;
        // MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 1) {
        //x[0] = 234;
    }
    
    MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
    
    

    printf("%d/%d: x[0] = %d\n", rank, comm_size, x[0]);
    printf("%d/%d: x[323] = %d\n", rank, comm_size, x[323]);
    
    MPI_Finalize();
    free(x);
    return 0;
}