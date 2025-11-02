#include "mpi.h" 
#include <stdio.h> 
#include <stdlib.h> 

int main (int argc, char *argv[]) { 
    int comm_size, rank, count, root; 
    int x[100], y[2]; 

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    count=rank;
    root = 2; 

    if (rank == root) {
        for (int i = 0; i < comm_size * 2; i++) {
            x[i] = i; //0 1 2 3 4 5 6 7 0 0 0 0 0 0 0 .... 0
        }
    }

    MPI_Scatter(x, 2, MPI_INT, y, 2, MPI_INT, root, MPI_COMM_WORLD); 
    
    printf("Rank = %d, y[0] = %d, y[1] = %d\n", rank, y[0], y[1]);
    
    MPI_Finalize(); 
}