#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int rank, size;
    MPI_Win win;
    int *buf = NULL;
    const int N = 16;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        buf = malloc(N * sizeof(int));
        for (int i = 0; i < N; i++) {
            buf[i] = -1;
        }

        MPI_Win_create(buf, N * sizeof(int), sizeof(int),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    } else {
        MPI_Win_create(NULL, 0, sizeof(int),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank != 0) {
        int value = rank;

        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);

        MPI_Put(&value, 1, MPI_INT,
                0, rank, 1, MPI_INT, win);

        MPI_Win_unlock(0, win);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            printf("buf[%d] = %d\n", i, buf[i]);
        }
    }

    if (rank == 0) {
        free(buf);
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}