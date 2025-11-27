#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int rank, size;
    MPI_Win win;
    int target_value = 0;
    int fetch_result = -1;
    int one = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        MPI_Win_create(&target_value, sizeof(int), sizeof(int),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    } else {
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }

    MPI_Win_fence(0, win);

    if (rank == 1) {
        MPI_Fetch_and_op(&one, &fetch_result, MPI_INT,
                         0, 0, MPI_SUM, win);
    }

    MPI_Win_fence(0, win);

    if (rank == 0) {
        printf("Rank 0: target_value = %d\n", target_value);
    }
    if (rank == 1) {
        printf("Rank 1: old target_value = %d\n", fetch_result);
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}
