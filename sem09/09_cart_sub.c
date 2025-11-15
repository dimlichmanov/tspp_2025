#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int dims[2] = {2, 3};
    int periods[2] = {0, 0}; 
    int reorder = 0;

    if (world_size != dims[0]*dims[1]) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cart_comm);

    int coords[2];
    int cart_rank;
    MPI_Comm_rank(cart_comm, &cart_rank);
    MPI_Cart_coords(cart_comm, cart_rank, 2, coords);

    /* Оставим столбцовое измерение, то есть у каждой процессной строки будет свой коммуникатор */  
    int remain_row[2] = {0, 1};
    MPI_Comm row_comm;
    MPI_Cart_sub(cart_comm, remain_row, &row_comm);

    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);

    /* Попытаемся посчитать сумму рангов по строкам */
    int row_sum = world_rank;
    int row_sum_result = 0;
    MPI_Reduce(&row_sum, &row_sum_result, 1, MPI_INT, MPI_SUM, 0, row_comm);

    /* Оставим строковое измерение, то есть у каждого процессного столбца будет свой коммуникатор */  
    int remain_col[2] = {1, 0}; 
    MPI_Comm col_comm;
    MPI_Cart_sub(cart_comm, remain_col, &col_comm);

    int col_rank, col_size;
    MPI_Comm_rank(col_comm, &col_rank);
    MPI_Comm_size(col_comm, &col_size);

    int col_sum = world_rank;
    int col_sum_result = 0;
    /* Попытаемся посчитать сумму рангов по столбцам */
    MPI_Reduce(&col_sum, &col_sum_result, 1, MPI_INT, MPI_SUM, 0, col_comm);


    /* Обратите внимание, что row_rank и col_rank - это нулевые процессы в своих коммуникаторах, 
    а не мировых, и они были выбраны как root для вышеприведённых коллективных операций*/
    if (row_rank == 0) {
        printf("ROW = %d, size=%d, ranks-sum=%d (coords row=%d)\n",
               coords[0], row_size, row_sum_result, coords[0]);
    }

    if (col_rank == 0) {
        printf("COL = %d, size=%d, ranks-sum=%d (coords col=%d)\n",
               coords[1], col_size, col_sum_result, coords[1]);
    }

    MPI_Comm_free(&row_comm);
    MPI_Comm_free(&col_comm);
    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}