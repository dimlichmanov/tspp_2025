#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    int comm_size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = 2000;
    int * global_array = NULL;

    // корневой процесс генерирует данные
    if (rank == 0){
        if (argc > 1){
            N = atoi(argv[1]);
        }

        // проверка, что N >= количества процессов
        if (N < comm_size){
            printf("Error: N (%d) must be >= number of process (%d)\n", N, comm_size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        global_array = (int *)malloc(N * sizeof(int));

        // генерируем последовательность
        srand(42);
        for (int i = 0; i < N; i++){
            global_array[i] = i; // неубывающая

            //но для теста добавим нарушения
            if (i == 8) global_array[i] = 5;
            if (i == 18) global_array[i] = 1;

            if (i == 4) global_array[i] = 10;  // На стыке между процессом 0 и 1

            if (i == 1000) global_array[i] = 950;

            if (i == 1990) global_array[i] = 1980;

            if (i == 1998) global_array[i] = 1990;

        }

        //вывод массива (только на процессе 0)
        printf("Generated array: ");
        for (int i = 0; i < N; i++){
            printf("%d ", global_array[i]);
        }
        printf("\n");
    }

    // рассылаем размер массива по всем процессам
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // вычисляем размер блока для каждого процесса
    int base = N / comm_size;
    int rem = N % comm_size;
    int local_n = base + (rank < rem ? 1 : 0); //точное кол-во элем-в для текущего процесса

    // вычисляем смещение каждого процесса
    int displ = 0;
    int sum_prefix = 0;
    MPI_Exscan(&local_n, &sum_prefix, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (rank != 0){
        displ = sum_prefix;
    }

    // подготовим массивы
    int * sendcounts = NULL;
    int * displs = NULL;

    if (rank == 0){
        sendcounts = (int*)malloc(comm_size * sizeof(int));
        displs = (int*)malloc(comm_size * sizeof(int));

        int offset = 0;
        for (int i = 0; i < comm_size; i++){
            sendcounts[i] = base + (i < rem ? 1 : 0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    // распределяем данные
    int * local_array = (int*)malloc(local_n * sizeof(int));
    MPI_Scatterv(global_array, sendcounts, displs, MPI_INT, local_array, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // проверяем свой блок на нарушение неубывания
    int local_violation = N + 1; // Значение "нарушение не найдено"

    for (int i = 0; i < local_n - 1; i++){
        if (local_array[i] > local_array[i + 1]){
            local_violation = displ + i; // глобальный индекс нарушения (ИСПРАВЛЕНО: было displ + 1)
            break;
        }
    }

    // Собираем все нарушения
    int global_inner_violation;
    MPI_Reduce(&local_violation, &global_inner_violation, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    // проверяем стыки между блоками
    // собираем граничные значения
    int first_elem = (local_n > 0) ? local_array[0] : 0;
    int last_elem = (local_n > 0) ? local_array[local_n - 1] : 0;
    
    int* first_elems = NULL;
    int* last_elems = NULL;

    if (rank == 0) {
        first_elems = (int*)malloc(comm_size * sizeof(int));
        last_elems = (int*)malloc(comm_size * sizeof(int));
    }

    MPI_Gather(&first_elem, 1, MPI_INT, first_elems, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&last_elem, 1, MPI_INT, last_elems, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // проверяем стыки на root и выдаем результат
    if (rank == 0) {
        int global_join_violation = N + 1;
        
        // Проверяем стыки между блоками
        for (int i = 0; i < comm_size - 1; i++) {
            if (last_elems[i] > first_elems[i + 1]) {
                // Нарушение на стыке блоков i и i+1
                int violation_index = displs[i] + sendcounts[i] - 1;
                if (violation_index < global_join_violation) {
                    global_join_violation = violation_index;
                }
            }
        }

        // Выбираем самое первое нарушение
        int final_violation = (global_inner_violation < global_join_violation) ? global_inner_violation : global_join_violation;

        // Выводим результат
        if (final_violation == N + 1) {
            printf("SUCCESS: Sequence is non-decreasing\n");
        } else {
            printf("VIOLATION: First non-decreasing violation at index %d\n", final_violation);
            printf("Elements: array[%d] = %d, array[%d] = %d\n", 
                   final_violation, global_array[final_violation],
                   final_violation + 1, global_array[final_violation + 1]);
        }

        // Освобождаем память
        free(first_elems);
        free(last_elems);
        free(sendcounts);
        free(displs);
        free(global_array);
    }

    free(local_array);
    MPI_Finalize();
    return 0;
}

