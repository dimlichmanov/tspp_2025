#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>

int main() {

    MPI_Init(NULL, NULL);

    int comm_size, rank;
    int *sendbuf;
    int root_rank = 0, rbuf[100]; 

    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int displs[comm_size];
    int scounts[comm_size];

    //int stride = 2;

    if (rank == root_rank) {
        sendbuf = (int*)malloc(400 * sizeof(int));
        for (int i = 0; i < 400; i++) {
            sendbuf[i] = i;
        }
    }

    // 0- 100  100 - 200 200 - 300 300 - 400

    for (int i = 0; i < comm_size; i++) {
        displs[i] = (comm_size - 1 - i) * 100;
        scounts[i] = 100; 
    }

    MPI_Scatterv(sendbuf, scounts, displs, MPI_INT, rbuf, 100, MPI_INT, root_rank, MPI_COMM_WORLD);

    printf("Rank %d got value rbuf[0] = %d\n", rank, rbuf[0]);

    MPI_Finalize();

    if (rank == root_rank) {
        free(sendbuf);
    }

}