// Написать параллельную программу. 
// Элементы квадратной матрицы распределены блочно по процессам. 
// Число процессов является квадратом целого числа.
// Начальное значение элементов задать случайно.
// Определить, является ли матрица симметричной.


#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    const int P = 3; 
    if (world_size != P * P) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int dims[2]     = { P, P };
    int periods[2]  = { 0, 0 };  
    int reorder     = 0;

    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cart_comm);

    int cart_size, cart_rank;

    MPI_Comm_rank(cart_comm, &cart_rank);
    MPI_Comm_size(cart_comm, &cart_size);

    int coords[2];
    MPI_Cart_coords(cart_comm, cart_rank, 2, coords);

    /* Получили мои координаты */
    int i = coords[0], j = coords[1];

    int A0[3][3] = {
        {1, 2, 3},
        {2, 5, 6},
        {3, 8, 9}
    };

    /* Нашли симметричный процесс и пытаемся понять, какой у него ранг 
    (это нужно, так как координаты в Sendrecv не передать) */
    int partner_coords[2] = { j, i };
    int partner_rank;
    MPI_Cart_rank(cart_comm, partner_coords, &partner_rank);

    int recvA[3][3];

    /* Тут можно, чтобы один из симметричных друг другу процессов делал проверку, но пусть оба сделают */
    MPI_Sendrecv(
        A0, 3 * 3, MPI_INT, partner_rank, 0,
        recvA, 3 * 3, MPI_INT, partner_rank, 0,
        cart_comm, MPI_STATUS_IGNORE
    );

    int symm = 1;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (A0[i][j] != recvA[j][i]) {
                symm=0;
            }
        }
    }

    int global_ok = 0;
    
    MPI_Allreduce(&symm, &global_ok, 1, MPI_INT, MPI_MIN, cart_comm);

    if (world_rank == 0) {
        if (global_ok == 1) {
            printf("OK\n");
        } else {
            printf("NOT OK\n");
        }
    }

    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}