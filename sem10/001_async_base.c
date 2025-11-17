#include <mpi.h>
#include <stdio.h>
#include <string.h>

/* в синхронном виде (когда Recv до Send в обоих процессах) - это чистый deadlock */

int main(int argc, char** argv) {
    int size, rank;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    MPI_Status status;
    
    char message[30];
    
    /* "Ручки" для проверки выполненности операций. У каждого процесса будет, естественно, своя копия ручки */
    MPI_Request req_send; 
    MPI_Request req_recv;

    /* Дальше замненяем на асинхронный приём сообщения */

    if (rank == 0) { 
        strcpy(message, "Hello");
        //MPI_Recv(message, 30, MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 

        MPI_Irecv(message, 30, MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &req_recv);
        MPI_Send(message, 30, MPI_CHAR, 1, 0, MPI_COMM_WORLD); 
        MPI_Wait(&req_recv, MPI_STATUS_IGNORE);

    } else if (rank == 1) { 
        strcpy(message, "World");
        MPI_Recv(message, 30, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        MPI_Send(message, 30, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } 
    
    printf( "%s\n", message);
    MPI_Finalize();

}