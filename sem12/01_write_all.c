#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/* Задание на коллективную операцию write_at_all */
int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_File fh;
    MPI_Status status;

    MPI_File_open(MPI_COMM_WORLD, "numbers.bin", MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
    int value_to_write = rank;

    MPI_Offset offset = rank * sizeof(int);

    MPI_File_write_at_all(fh, offset, &value_to_write, 1, MPI_INT, &status);
    MPI_File_sync(fh);
    // NOOO MPI_Barrier(MPI_COMM_WORLD);

    int value_read;
    MPI_File_read_at_all(fh, offset, &value_read, 1, MPI_INT, &status);

    printf("Rank %d: to_write %d, to_read %d\n", rank, value_to_write, value_read);

    MPI_File_close(&fh);
    MPI_Finalize();
    return 0;
}