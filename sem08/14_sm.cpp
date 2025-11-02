#include <mpi.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>

/* Пытаемся посчитать распределённо softmax

https://pica.zhimg.com/v2-b4f8b8e06aa83cfd061c005a640abcdc_1440w.jpg

То есть, у нас xi - элементы  нашего вектора.
Сначала нужно найти максимум, затем посчитать сумму экспонент для определения знаменателя, а потом уже провести деление для каждого элемента 

сложим результат отдельно, в другом векторе

*/

int main(int argc, char** argv) {
    int rank, comm_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int displs[comm_size];
    int scounts[comm_size];

    int N; 
    if (rank == 0) {
        if (argc > 1) N = atoi(argv[1]);
        else N = 16; 
    }   

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int base = N / comm_size;
    int remain = N % comm_size;

    int local_n = base + ((rank < remain) ? 1 : 0);

    std::vector<double> all_values;
    std::vector<double> my_values(local_n);


    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    /* На rank = 0 считаем сдвиги - записываем и прибавляем размер вектора для нового процесса. Кстати, Exscan для распределённых элементов делает то же самое */
    if (rank == 0) {
        all_values.resize(N);
        int offset_temp = 0;
        for (int r = 0; r < comm_size; ++r) {
            int cnt = base + (r < remain ? 1 : 0);
            scounts[r] = cnt;
            displs[r] = offset_temp; 

            offset_temp += cnt;
        }

        for (int i = 0; i < N; i++) {
            all_values[i] = (double)rand() / RAND_MAX;
        }
    }

    /* Знаем размеры, нужные каждому ранку, знаем, откуда начинаются в all_values их элементы. */
    MPI_Scatterv(all_values.data(), scounts, displs, MPI_DOUBLE, my_values.data(), local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);


    /* Ищем максимум, сначала локально, затем через Allreduce понимаем глобальный максимум */
    double local_max = *std::max_element(my_values.begin(), my_values.end());
    double global_max = 0.0;
    MPI_Allreduce(&local_max, &global_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    /* Теперь начинаем считать знаменатель - нам нужна сумма всех экспонент, сначала локально, потом глобально */
    double local_sum = 0.0;
    for (double v : my_values) {
        local_sum += std::exp(v - global_max);
    }
    double global_sum = 0.0;
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    std::vector<double> y(local_n); // Тут для каждого процесса будет лежать нужный результат

    if (global_sum > 0.0) {
        // Просто делим каждую экспоненту на найденный знаменатель
        for (int i = 0; i < local_n; ++i) {
            y[i] = std::exp(my_values[i] - global_max) / global_sum;
        }
    }

    // Заводим вектор, чтобы туда сложить глобальный результат
    std::vector<double> global_softmax;
    
    if (rank == 0) {
        global_softmax.resize(N);
    }

    // Сдвиги и размеры в global_softmax будут такими же, как и у all_values, когда отправяли элементы по процессам. Поэтому используем scounts и displs для сбора

    MPI_Gatherv(y.data(), local_n, MPI_DOUBLE, global_softmax.data(), scounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0) {
        for (double elem: global_softmax) {
            std::cout << elem << " ";
        }
        std::cout << std::endl << "Elapsed in " << end - start << " seconds" << std::endl;

        /* Сумма вероятностей должна равняться 1 */
        double check_prob_ver = std::accumulate(global_softmax.begin(), global_softmax.end(), 0.0);
        std::cout << "Prob sum: " << check_prob_ver << std::endl;
    }


    MPI_Finalize();
    return 0;
}