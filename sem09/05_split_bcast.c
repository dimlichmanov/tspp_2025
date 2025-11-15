#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int myid, numprocs;
    int color, new_id, new_nodes;
    MPI_Comm new_Comm;
    int broad_val = -1;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    
    /* Цвет - столбец матрицы */
    color = myid % 4;
    
    MPI_Comm_split(MPI_COMM_WORLD, color, myid, &new_Comm);

    MPI_Comm_rank(new_Comm, &new_id);
    MPI_Comm_size(new_Comm, &new_nodes);
    
    if (new_id == 0) broad_val = color;
    
    /* Хоть мы и создали столбцовые коммуникаторы, ничего нам не мешает общаться через мировой (учавствуют по-прежнему все процессы)*/
    MPI_Bcast(&broad_val, 1, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Old proc %d has new rank %d and received value %d\n", myid, new_id, broad_val);
    MPI_Finalize();
}