// Написать параллельную программу на MPI.
// Элементы целочисленного вектора распределены равномерно по процессам. 
// Размер вектора задается в командной строке. Элементы генерируется процессами случайными числами.

// Сдвинуть элементы вектора циклически так, чтобы первым элементом стал максимальный элемент вектора. 
// Если элементов с максимальным значением несколько, сдвиг выполнять относительно первого элемента.

// Результат вывести, обеспечив порядок следования элементов по номерам процессов.
// Решить задачу без использования дополнительного массива. 
// Пересылать все массивы в один процесс нельзя.

// Компилировать через mpic++ 

#include "mpi.h"
#include <vector>
#include <limits>
#include <algorithm>

#define MY_DUMMY_TAG 1

/* Эта функция пытается напечатать всё по-очереди, но не всегда это получается */
void print_in_rank_order(const std::vector<int>& a, int rank, int size)
{
    MPI_Barrier(MPI_COMM_WORLD);
    for (int r = 0; r < size; ++r) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (r == rank) {
            std::cout << "[rank " << rank << "] " << ":";
            for (int v : a) std::cout << ' ' << v;
            std::cout << std::endl << std::flush;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
}


int main(int argc, char** argv) {
    
    int rank, comm_size;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    
    //У каждого процесса есть возможность читать с командной строки

    int vec_size = (argc > 1) ? atoi(argv[1]) : 20;

    /* Допускаем целое деление */
    int local_size = vec_size / comm_size;

    /* У каждого процесса свой кусок вектора, здесь и далее везде в таких задачах полагаем (не ограничивая общности),  
    что начало глобального вектора лежит на нулевом процессе, конец - на последнем */
    std::vector<int> my_part(local_size);

    srand(rank + 323 + 13);
    for (int& value: my_part) value  = rand() % 1000;

    print_in_rank_order(my_part, rank, comm_size);

    int local_max = my_part[0];
    int max_local_pos = 0;

    /* Считаем локальный максимум и его позицию на каждом процессе. */
    int temp_pos = 0;
    for (int value: my_part) { 
        if (local_max < value) {
            local_max = value;
            max_local_pos = temp_pos;
        }
        temp_pos++;
    }

    int elems[2];
    elems[0] = local_max;
    elems[1] = max_local_pos + local_size * rank;

    int final_elems[2];

    /* Allreduce нам вернёт максимум и позицию, соответствующую этому */
    /* Позиция не в терминах индекса элемента. Под позицией понимается то, что лежит в elems[1]. При равенстве максимумов берётся минимальный elems[1]. */
    MPI_Allreduce(elems, final_elems, 1, MPI_2INT, MPI_MAXLOC, MPI_COMM_WORLD);

    int final_pos;
    final_pos = final_elems[1];

    // Ручной поиск максимума, позиции и рассылка элементов нам больше не нужны
    
    // if (rank == 0) {
    //     int info[2];
    //     for (int nb = 1; nb < comm_size; nb++) {
    //         /* Пусть нулевой процесс (rank == 0) соберёт все локальные максимумы и позиции 
    //         и попытается обновить свой максимум и позицию при получении сообщения от каждого соседа */
    //         MPI_Recv(&info, 2, MPI_INT, nb, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //         if (info[0] > local_max) {
    //             local_max = info[0];
    //             max_local_pos = info[1] + (nb * local_size); // Поскольку каждый процесс отправил туда локальную позицию, мы должны тут взять глобальную 
    //                                                          // (иначе будем должны ещё и владельца максимума отдельно собирать)
    //         }
    //     }
    // } else {
    //     int info[2];
    //     /* Каждый процесс передаёт нулевому свои найденные максимум и позицию */
    //     info[0] = local_max;
    //     info[1] = max_local_pos;
    //     MPI_Send(&info, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
    // }

    // // К этому моменту у нулевого процесса local_max и max_local_value - глобальные, у остальных - свои локальные

    // // Надо, чтобы каждый процесс знал позицию максимума, само значение уже не нужно
    // int final_pos;

    // if (rank == 0) {
    //     /* Пусть нулевой процесс всем отправит это значение, себе просто скопирует в него max_local_pos */
    //     final_pos = max_local_pos;
    //     for (int nb = 1; nb < comm_size; nb++) {
    //         MPI_Send(&max_local_pos, 1, MPI_INT, nb, 1, MPI_COMM_WORLD);
    //     }

    // } else {
    //     MPI_Recv(&final_pos, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // }

    // нам нужно знать только локальную позицию, поскольку только она влияет на размеры пересылаемых кусков дальше
    // local_shift у всех один и тот же
    int local_shift = final_pos % local_size; 

    // нам нужно знать владельца, поскольку он передаст свой кусок [final_pos, local_size) нулевому процессу
    // Это сформирует схему пересылки (если max_owner == 2, куски [final_pos, local_size) у всех процессов перешлются левому соседу "через одного")
    int max_owner = final_pos / local_size;  

    // Мы знаем теперь, какими кусками обмениваться, можем вычислить адресатов и отправителей сообщений для каждого процесса 

    // от кого будем получать данные
    // [0] - для кусков [0, final_pos), [1] - для кусков [final_pos, local_size)
    int proc_2_get[2];

    // Обмены циклические (rank == 0 - правый сосед для rank == comm_size - 1), поэтому операции по модулю
    // Получаем элементы справа, поэтому прибавляем max_owner и max_owner + 1
    proc_2_get[0] = (rank + max_owner + 1) % comm_size;
    proc_2_get[1] = (rank + max_owner) % comm_size;

    // кому будем отправлять данные
    int proc_2_give [2];

    // Отправляем налево, поэтому вычитаем.
    // Не забываем про то, что внутренняя скобка может быть < 0 (обмены циклические), поэтому нужно прибавить comm_size для получения неотрицательного номера процесса
    proc_2_give[0] = ((rank - max_owner - 1) + comm_size) % comm_size;
    proc_2_give[1] = ((rank - max_owner) + comm_size) % comm_size;
   

    // Знаем что отправлять, знаем кому. Отправляем

    // отправляем свой [0, local_shift) налево и принимаем справа
    MPI_Sendrecv_replace(my_part.data(), local_shift, MPI_INT, proc_2_give[0], MY_DUMMY_TAG, proc_2_get[0], MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // отправляем [local_shift, local_size) налево и принимаем справа
    MPI_Sendrecv_replace(my_part.data() + local_shift, local_size - local_shift, MPI_INT, proc_2_give[1], MY_DUMMY_TAG, proc_2_get[1], MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    //Переставляем локально уже новые данные 
    std::reverse(my_part.begin(), my_part.begin() + local_shift);
    std::reverse(my_part.begin() + local_shift, my_part.end());
    std::reverse(my_part.begin(), my_part.end());

    print_in_rank_order(my_part, rank, comm_size);

    MPI_Finalize();
}