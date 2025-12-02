
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    MPI_File fh;
    MPI_Status status;
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int local_n = 4;
    int *buf = (int*)malloc(local_n * sizeof(int));

    MPI_File_open(MPI_COMM_WORLD, "interleaved.bin", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    MPI_Offset disp = (MPI_Offset)rank * local_n * sizeof(int);

    /* Тут одинаковые etype и filetype и они элементарные типы. Тут view не делает ничего осмысленного, кроме установки смещения */
    MPI_File_set_view(fh, disp, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);

    MPI_File_read_all(fh, buf, local_n, MPI_INT, &status);

    printf("Rank %d: ", rank);
    for (int i = 0; i < local_n; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");

    free(buf);
    MPI_File_close(&fh);
    MPI_Finalize();
    return 0;
}