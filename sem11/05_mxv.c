#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void compute_local_rows(int N, int size, int rank,
                        int *local_rows, int *start_row)
{
    int base = N / size;
    int rem  = N % size;

    if (rank < rem) {
        *local_rows = base + 1;
        *start_row  = rank * (*local_rows);
    } else {
        *local_rows = base;
        *start_row  = rem * (base + 1) + (rank - rem) * base;
    }
}

void fill_local_matrix(double *A_local, int local_rows, int N, int start_row)
{
    for (int i = 0; i < local_rows; i++) {
        int global_i = start_row + i;
        for (int j = 0; j < N; j++) {
            A_local[i * N + j] = (double)(global_i + 1 + j + 1);
        }
    }
}

void fill_vector_b(double *b, int N)
{
    for (int i = 0; i < N; i++) {
        b[i] = 1.0;
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = atoi(argv[1]);

    int local_rows, start_row;
    compute_local_rows(N, size, rank, &local_rows, &start_row);

    double *A_local = NULL;
    if (local_rows > 0) {
        A_local = (double *)malloc((size_t)local_rows * N * sizeof(double));
    }

    fill_local_matrix(A_local, local_rows, N, start_row);

    double *b = NULL;
    MPI_Win win_b;

    if (rank == 0) {
        b = (double *)malloc(N * sizeof(double));
        fill_vector_b(b, N);
        MPI_Win_create(b, N * sizeof(double), sizeof(double),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win_b);
    } else {
        MPI_Win_create(NULL, 0, sizeof(double),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win_b);

        b = (double *)malloc(N * sizeof(double));
    }

    MPI_Win_fence(0, win_b);

    if (rank != 0) {
        MPI_Get(b, N, MPI_DOUBLE,
                0, 0, N, MPI_DOUBLE,
                win_b);
    }

    MPI_Win_fence(0, win_b);

    double *c_local = NULL;
    if (local_rows > 0) {
        c_local = (double *)malloc(local_rows * sizeof(double));

        for (int i = 0; i < local_rows; i++) {
            double sum = 0.0;
            for (int j = 0; j < N; j++) {
                sum += A_local[i * N + j] * b[j];
            }
            c_local[i] = sum;
        }
    }

    MPI_Win_free(&win_b);
    free(b);

    double *c = NULL;
    MPI_Win win_c;

    if (rank == 0) {
        c = (double *)malloc(N * sizeof(double));
        MPI_Win_create(c, N * sizeof(double), sizeof(double),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win_c);
    } else {
        MPI_Win_create(NULL, 0, sizeof(double),
                       MPI_INFO_NULL, MPI_COMM_WORLD, &win_c);
    }

    MPI_Win_fence(0, win_c);

    if (local_rows > 0) {
        MPI_Put(c_local, local_rows, MPI_DOUBLE,
                0, start_row, local_rows, MPI_DOUBLE,
                win_c);
    }

    MPI_Win_fence(0, win_c);

    if (rank == 0) {
        printf("Result vector c (first min(N, 10) elements):\n");
        int limit = (N < 10) ? N : 10;
        for (int i = 0; i < limit; i++) {
            printf("c[%d] = %f\n", i, c[i]);
        }
    }

    MPI_Win_free(&win_c);

    if (A_local)  free(A_local);
    if (c_local)  free(c_local);
    if (rank == 0 && c) free(c);

    MPI_Finalize();
    return 0;
}
