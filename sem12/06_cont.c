#include <stdio.h>
#include <mpi.h>

/* Пример, когда элементарным типом может быть производный тип */
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_File fh;
    MPI_Datatype record_type;
    int n_local = 2;      

    MPI_Type_contiguous(2, MPI_INT, &record_type);
    MPI_Type_commit(&record_type);

    int buf[2 * n_local];
    for (int i = 0; i < n_local; i++) {
        buf[2*i] = rank; 
        buf[2*i + 1] = i; 
    }

    MPI_File_open(MPI_COMM_WORLD, "data.bin", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

    /* Тут говорим, что из каждого cont-элемента собираем ещё cont, то есть в итоге 4 элемента */
    MPI_Datatype etype    = record_type;
    MPI_Datatype filetype = record_type;

    MPI_Offset disp = (MPI_Offset)rank * n_local * 2 * sizeof(int);

    MPI_File_set_view(fh, disp, etype, filetype,"native", MPI_INFO_NULL);

    MPI_File_write_all(fh, buf, n_local, record_type, MPI_STATUS_IGNORE);

    MPI_File_close(&fh);
    MPI_Type_free(&record_type);

    MPI_Finalize();
    return 0;
}