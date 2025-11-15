#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <set>
#include <vector>

/* Пытаемся вручную работать с номерами процессов и запихать их в группу, которой даём свой коммуникатор для общения. 
   То есть процесс в группе может общаться как через MPI_COMM_WORLD (со всеми процессами), так и через новый коммуникатор */
int main(int argc, char **argv) {
    MPI_Init(NULL, NULL);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int n = 7;
    std::vector<int> ranks{1, 2, 5, 7, 11, 12, 13};

    std::set<int> rank_set(ranks.begin(), ranks.end());

    const int n_delete = 2;
    const int ranks_2_delete[2] = {0, 3};

    MPI_Group elite_group;
    MPI_Group_incl(world_group, n, ranks.data(), &elite_group);

        
    // MPI_Comm_create_group коллективна для всех процессов группы, MPI_Comm_create - для всех процессов старого коммуникатора 
    MPI_Comm elite_comm = MPI_COMM_NULL;
    
    /* Альтернативный подход */
    
    // if (rank_set.find(world_rank) != rank_set.end()) {
    //     MPI_Comm_create_group(MPI_COMM_WORLD, elite_group, 0, &elite_comm);
    // }

    MPI_Comm_create(MPI_COMM_WORLD, elite_group, &elite_comm);

    int elite_rank = -1, elite_size = -1;
    
    if (elite_comm != MPI_COMM_NULL) {
        MPI_Comm_rank(elite_comm, &elite_rank);
        MPI_Comm_size(elite_comm, &elite_size);
    }

    printf("WORLD RANK/SIZE: %d/%d --- PRIME RANK/SIZE: %d/%d\n",
    world_rank, world_size, elite_rank, elite_size);

    MPI_Group_free(&world_group);
    MPI_Group_free(&elite_group);

    if (elite_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&elite_comm);
    }

    MPI_Finalize();
}