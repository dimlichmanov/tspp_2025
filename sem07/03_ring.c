#include <stdio.h>
#include "mpi.h"

#include "stdlib.h"

int main(int argc, char **argv)
{
    int rank, value, size;
    MPI_Status status;

    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
	
    int x, y;
    int* b = (int*) malloc(323000 * sizeof(int));
    int* c = (int*) malloc(323000 * sizeof(int));

    if (rank == 0) {
        x = 400; 
        for (int i = 0; i < 323000; i++) {
                b[i] = i;
        }
        /* такой порядок: Recv() Send() в обоих процессах - deadlock! 
           Send() Recv() - ОK только для небольших сообщений (помним про системный буфер), но лучше так не писать

           Или разносим Recv() Send() ---  Send() Recv(), или пишем Sendrecv() */

        //MPI_Send(b, 323000, MPI_INT, 1, 0, MPI_COMM_WORLD );
        //MPI_Recv(c, 323000, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Sendrecv(b, 323000, MPI_INT, 1, 0, c, 323000, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
	else
    {
        y = 600;
        for (int i = 0; i < 323000; i++) {
                c[i] = i;
        }
        // MPI_Recv(b, 323000, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // MPI_Send(c, 323000, MPI_INT, 0, 0, MPI_COMM_WORLD );

        MPI_Sendrecv(c, 323000, MPI_INT, 0, 0, b, 323000, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	printf( "Process %d has x = %d and y =  %d\n", rank, b[656], c[537]);

    MPI_Finalize();
    return 0;
}