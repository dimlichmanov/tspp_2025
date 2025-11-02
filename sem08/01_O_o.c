#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    int size, rank;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    MPI_Status status;
    
    char message[30];
    // ошибка - deadlock
    if (rank == 0) { 
        strcpy(message, "Hello");
        MPI_Recv(message, 30, MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        MPI_Send(message, 30, MPI_CHAR, 1, 0, MPI_COMM_WORLD); 

    } else if (rank == 1) { 
        strcpy(message, "World");
        MPI_Recv(message, 30, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        MPI_Send(message, 30, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } 
    
    printf( "%s\n", message);
    MPI_Finalize();

}