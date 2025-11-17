#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// mpirun -np 8 --oversubscribe ./a.out

// В этой задаче мы хотим каждому из 8 процессов раздать по своему столбцу коллективной операцией

// Как использовать производные типы с коллективными операциями? 
// Когда мы писали на прошлых занятиях MPI_Scatter и прочие, мы говорили, 
// что корневой процесс берёт соседние значения и рассылает всем по очереди
// То есть было 100 интов и 10 процессов, MPI_Scatter(.., 10, MPI_INT,... ) - каждый процесс получил свою десятку значений

// На самом деле, каждая посылка значения корневым процессом другим процессам происходит, начиная с элемента 
// sendbuf + i * sendcount * extent(sendtype)
// то есть - начало посылки для i+1 элемента начинается сразу после конца посылки для i
// Поскольку у MPI_INT и прочих extent = 1, всё замечательно.
// Когда мы хотим отправить каждому процессу свою полоску (тип Contigious), всё тоже ок - полоски идут друг за другом

// Но когда мы отправляем вектор, у него extent включает в себя паддинг (для столбца в матрице 8 на 8 равен 57), 
// поэтому при наивном использовании MPI_Scatter будет ошибка! MPI полезет искать элементы для отправки, 
// начиная с 57 элемента, и совершенно точно выйдет за пределы массива.

// Чтобы каждый процесс получил свою колонку, нам нужно ИСКУССТВЕННО сделать extent = 1 
// (чтобы столбец, отправляемый процессу i + 1, начинался рядом со столбцом для процесса i)
// У вас создастся новый, искусственный тип, который конечно не показывает реальный extent, но очень удобен для коллективных операций

int main(int argc, char* argv[])
{
    int buffer[8][8];
    int recv_buf[8];

    MPI_Init(&argc, &argv);
    int size, my_rank;
    MPI_Aint lb, sz;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Datatype column_type;
    // Тут всё по-прежнему - столбец матрицы
    MPI_Type_vector(8, 1, 8, MPI_INT, &column_type);

    MPI_Datatype column_type_new;
    // 0 - нижняя граница типа
    // 1 * sizeof(int) - новый extent для колонки
    MPI_Type_create_resized(column_type, 0, 1 * sizeof(int), &column_type_new);

    MPI_Type_get_extent(column_type_new, &lb, &sz);
    MPI_Type_commit(&column_type_new);

    if (my_rank == 0) {
        printf("LB: %ld, extent: %ld\n", lb, sz);
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                buffer[i][j] = i * 8 + j;
            }
        }
    }
    else {
       for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                buffer[i][j] = -1;
            }
        }
    }

    /* Сюда передаём новый тип столбца и MPI будет знать, что новый extent = 1 */
    MPI_Scatter(buffer, 1, column_type_new, recv_buf, 8, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 6) {
        for (int j = 0; j < 8; j++) {
            printf("%d ", recv_buf[j]);
        }
    }
    MPI_Type_free(&column_type_new);
    MPI_Type_free(&column_type);
    MPI_Finalize();
    return EXIT_SUCCESS;
}