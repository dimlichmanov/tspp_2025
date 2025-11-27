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

    int my_value = rank;
    MPI_Win win;
    MPI_Win_create(&my_value, sizeof(int), sizeof(int), MPI_INFO_NULL,
                   MPI_COMM_WORLD, &win);

    MPI_Win_fence(0, win);

    int target = (rank + 1) % size;
    MPI_Put(&my_value, 1, MPI_INT, target, 0, 1, MPI_INT, win);

    MPI_Win_fence(0, win);

    int source_rank = (rank == 0) ? size - 1 : rank - 1;
    printf("Process %d got %d from %d\n",
           rank, my_value, source_rank);

    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}