#include <mpi.h>
#include <stdio.h>
#include <vector>
using MyGraph = std::vector<std::vector<int>>;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    if (comm_size < 4) MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);


    // 6 8 8 6

    MyGraph g = {
        {1,3},   // 0  + 6    
        {0,2,4}, // 1  + 6
        {1,5},   // 2 
        {0,4},   // 3
        {1,3,5}, // 4
        {2,4}    // 5   
    };

    if (rank == 1) {
        g.push_back({0,1}); // 6
        g.push_back({5,1}); // 7 + 6
    }
    
    if (rank == 2) {
        g.push_back({4,3}); // 6
        g.push_back({5,3}); // 7
    }

    int my_offset = 0;
    int local_graph_size = g.size();


    // Удобно для подсчёта сдвигов, когда мы хотим понять, какой глобальный индекс наших элементов
    // Потому что не всегда, как в задаче с блоками elems[1] = max_local_pos + local_size * rank;, нам удаётся так просто посчитать глобальный индекс нашего элемента
    
    MPI_Exscan(&local_graph_size, &my_offset, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    printf("rank %d, offset = %d\n", rank, my_offset);

    MPI_Finalize();
    return 0;
}