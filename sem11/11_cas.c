#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        MPI_Finalize(); 
        return 0;
    }

    int counter = 0;

    MPI_Win win;
    MPI_Win_create(&counter,
                   sizeof(int),
                   sizeof(int),
                   MPI_INFO_NULL,
                   MPI_COMM_WORLD,
                   &win);

    if (rank == 0) {
        counter = 0;
        printf("Initital counter %d\n", counter);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    const int NUM_INCR = 5;

    if (rank != 0) {
        MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);

        for (int k = 0; k < NUM_INCR; ++k) {
            int expected; 
            int desired;
            int actual;   

            int success = 0;

            while (!success) {
                MPI_Fetch_and_op(NULL, &expected, MPI_INT,
                                 0, 0, MPI_NO_OP, win);
                MPI_Win_flush(0, win);

                desired = expected + 1;
                
                MPI_Compare_and_swap(&desired, &expected, &actual,
                                     MPI_INT, 0, 0, win);
                MPI_Win_flush(0, win);

                if (actual == expected) {
                    success = 1; 
                }
            }
        }

        MPI_Win_unlock(0, win);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Final counter = %d (expected %d)\n", counter, (size - 1) * NUM_INCR);
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}