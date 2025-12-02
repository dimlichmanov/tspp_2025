#include <mpi.h>
#include <stdio.h>

/* Упорядоченная запись */
int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_File fh;
    MPI_Status status;

    MPI_File_open(MPI_COMM_WORLD, "output.txt", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

    char buf[128];
    int len = snprintf(buf, sizeof(buf), "Hello from rank %d\n", rank);

    MPI_File_write_ordered(fh, buf, len, MPI_CHAR, &status);

    MPI_File_close(&fh);
    MPI_Finalize();
    return 0;
}