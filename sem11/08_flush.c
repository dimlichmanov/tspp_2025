#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int rank, size;
    MPI_Win win;
    double *sum = NULL;
    const int STEPS = 3;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        MPI_Finalize();
        return 0;
    }
    
    if (rank == 0) {
        sum = malloc(sizeof(double));
        *sum = 0.0;
        MPI_Win_create(sum, sizeof(double), sizeof(double),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    } else {
        MPI_Win_create(NULL, 0, sizeof(double),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }

    MPI_Win_lock_all(0, win);

    for (int step = 0; step < STEPS; ++step) {

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank != 0) {
            double contrib = (double)rank;

            MPI_Accumulate(&contrib, 1, MPI_DOUBLE,
                           0, 0, 1, MPI_DOUBLE, MPI_SUM, win);

            /* Ожидание завершения НАШИХ (от origin) записей.*/
            MPI_Win_flush(0, win);
        }

        /* Барьер тут для красоты вывода. Он сам по себе не завершает RMA-операции. 
        Мы просто тут убедились, что все процессы завершили свои операции и пришли сюда */
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            double current;
            
            /* Стучаться в своё окно можно и так. */
            MPI_Get(&current, 1, MPI_DOUBLE,
                    0, 0, 1, MPI_DOUBLE, win);

            /* Поскольку все операции уже к этому моменту точно завершились, flush можно убрать */
            MPI_Win_flush(0, win);

            printf("step %d: sum = %f\n", step, current);
        }
    }
    MPI_Win_unlock_all(win);

    MPI_Win_free(&win);
    if (rank == 0) {
        free(sum);
    }

    MPI_Finalize();
    return 0;
}