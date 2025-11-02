#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int tag, x, rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    MPI_Status stat;

    tag=rank;
    x=rank; 

    if (rank == 0) { 
        MPI_Send(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&x, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("I am rank %d, x = %d\n", rank, x); 
    }
    if (rank == 1) {
        MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // тут ждём сообщение с тэгом 1, а нулевой его не отправит
        MPI_Recv(&x, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        printf("I am rank %d, x = %d\n", rank, x); 
    }
    
    MPI_Finalize();
}