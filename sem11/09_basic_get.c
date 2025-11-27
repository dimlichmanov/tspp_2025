#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int my_value = rank;

    MPI_Win win;
    MPI_Win_create(&my_value, sizeof(int), sizeof(int),             
        MPI_INFO_NULL,           
        MPI_COMM_WORLD,          
        &win                     
    );

    int *all_values = NULL;
    if (rank == 0) {
        all_values = malloc(size * sizeof(int));
        all_values[0] = my_value;
    }

    MPI_Win_fence(0, win);

    if (rank == 0) {
        for (int other_rank = 1; other_rank < size; ++other_rank) {
            MPI_Get(&all_values[other_rank], 1, MPI_INT, other_rank, 0, 1, MPI_INT, win);
        }
    }

    MPI_Win_fence(0, win);

    if (rank == 0) {
        for (int i = 0; i < size; ++i) {
            printf("%d ", all_values[i]);
        }
        printf("\n");
        free(all_values);
    }

    MPI_Win_free(&win);

    MPI_Finalize();
    return 0;
}
