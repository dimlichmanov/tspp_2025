#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NLOCAL 4 

/* Воспринимаем файл в виде вектора */
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int i;
    int *buf = malloc(NLOCAL * sizeof(int));
    for (i = 0; i < NLOCAL; i++) {
        buf[i] = rank * 100 + i;
    }

    MPI_Datatype filetype;
    int count = NLOCAL;
    int blocklength = 1;
    int stride = size;

    MPI_Type_vector(count, blocklength, stride, MPI_INT, &filetype);
    MPI_Type_commit(&filetype);

    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, "interleaved.bin", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);

    MPI_Offset disp = rank * sizeof(int);

    MPI_File_set_view(fh, disp, MPI_INT, filetype, "native", MPI_INFO_NULL);

    MPI_Status status;
    MPI_File_write_all(fh, buf, NLOCAL, MPI_INT, &status);

    MPI_File_close(&fh);
    MPI_Type_free(&filetype);
    free(buf);
    MPI_Finalize();
    return 0;
}