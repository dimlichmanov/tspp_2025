#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/* */
int main(int argc, char **argv)
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status status;

    if (size == 2) { // интересуют только случаи, когда запустили программу на двух процессах 
        if (rank == 0) {
            int x = 3243;
            int* a = (int*) malloc(323 * sizeof(int));
            for (int i = 0; i < 323; i++) {
                a[i] = i;
            }

            MPI_Send(a, 323, MPI_INT, 1, 99, MPI_COMM_WORLD); 
            //MPI_Send(&x, 1, MPI_INT, 1, 90, MPI_COMM_WORLD);

            free(a);
        } else { // то есть rank == 1
            int y = 0;
            int* b = (int*) malloc(323 * sizeof(int));

            /* Необходимо также отметить, что второй параметр Recv - МАКСИМАЛЬНОЕ число элементов для приёмки. 
            То есть запросив 323 элемента, можем принять сколь угодно меньше (например, отправку нулевым процессом x) */

            MPI_Recv(b, 323, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //MPI_Recv(&y, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            //printf("y = %d\n", y);
            for (int i = 0; i < 323; i++) {
                printf("%d ", b[i]);
            }
        }
    }
    
    MPI_Finalize();
    return 0;
}