#include <stdio.h>
#include <mpi.h>


int main(int argc, char **argv)
{
    int np, rank; // локальные переменные для каждого процесса
    
    printf("Hello before Init! \n"); //выведется столько, сколько процессов у нас (np)

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &np); // np - размер коммуникатора 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // MPI даёт нам уникальный номер от 0 до np - 1

    printf("%d/%d, hello\n", rank, np);
    
    MPI_Finalize();
    return 0;
}