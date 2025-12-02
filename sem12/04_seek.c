#include <mpi.h>
#include <stdio.h>

/* Чтение через установку указателя */
int main(int argc, char **argv) {
    MPI_File fh;
    MPI_Status status;
    int rank, size;
    int value;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_File_open(MPI_COMM_WORLD, "numbers.bin", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    MPI_Offset offset = (MPI_Offset)rank * sizeof(int);

    MPI_File_seek(fh, offset, MPI_SEEK_SET);
    /* Следите, чтобы записи были в разные участки файла */
    MPI_File_read(fh, &value, 1, MPI_INT, &status);

    printf("Rank %d reads %d\n", rank, value);

    MPI_File_close(&fh);
    MPI_Finalize();
    return 0;
}