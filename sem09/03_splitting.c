#include <mpi.h>
#include <stdio.h>

/* Мы можем разбивать процессы по коммуникаторам и без явной работы с группами */
int main(int argc, char **argv) {
    MPI_Init(NULL, NULL);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int color = world_rank / 4;

    /* Процессы, у которых одинаковый цвет, попадают в одну группу и получают свой коммуникатор для общения */
    /* То есть при запуске на 16 процессах у нас получается 4 новых коммуникатора (каждый на свою строку) */
    /* Ранг процесса в новом коммуникаторе зависит от предпоследнего параметра
    Процессы одного цвета получают ранг по возрастанию этого параметра (обычно хватает передачи world_rank), 
    тут в примере инвертируем этот порядок*/ 
    
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_size - 1 - world_rank, &row_comm);

    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);


    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    printf("WORLD RANK/SIZE: %d/%d --- ROW RANK/SIZE: %d/%d\n",
    world_rank, world_size, row_rank, row_size);

    MPI_Comm_free(&row_comm);

    MPI_Finalize();
}