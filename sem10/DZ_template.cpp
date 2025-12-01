#include <mpi.h>
#include <vector> 

#define MY_DUMMY_TAG 0

int main(int argc, char** argv) {
    int rank, comm_size;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    
    /* Будем создавать топологию процессов, так удобнее */
    int cart_dims[3] = {0, 0, 0};

    /* Пусть MPI нам сам подберёт измерения */
    MPI_Dims_create(comm_size, 3, cart_dims);

    if (rank == 0) {
        std::cout << cart_dims[0] << " " << cart_dims[1] << " " << cart_dims[2] << std::endl;
    }

    int N_total = atoi(argv[1]);

    /* Размеры локального блока */
    int x_local = N_total/cart_dims[0];
    int y_local = N_total/cart_dims[1];
    int z_local = N_total/cart_dims[2];

    int periodic[3] = {0, 0, 0};
    int reorder = 0;

    MPI_Comm cart_comm;

    MPI_Cart_create(MPI_COMM_WORLD, 3, cart_dims, periodic, reorder, &cart_comm);

    int left_neigh, right_neigh, down_neigh, up_neigh, back_neigh, front_neigh;

    MPI_Cart_shift(cart_comm, 0, 1, &left_neigh, &right_neigh);
    MPI_Cart_shift(cart_comm, 1, 1, &down_neigh, &up_neigh);
    MPI_Cart_shift(cart_comm, 2, 1, &back_neigh, &front_neigh);

    int cart_rank;
    MPI_Comm_rank(cart_comm, &cart_rank);

    /* Для демонстрации того, как MPI раскидывает процессы по топологии */
    if (cart_rank == 0) {
        std::cout << cart_rank << " down:" << down_neigh << " up:" << up_neigh << " left:" << left_neigh << 
        " right:" << right_neigh << " back:" << back_neigh << " front:" << front_neigh << std::endl;   
    }

    /* Не будем выделять +2 по каждому измерению, выделим буфера для приёмки отдельно
       Можете в своём решении сделать иначе */
    std::vector<double> local_grid(x_local * y_local * z_local);
    std::vector<double> local_grid_tmp(x_local * y_local * z_local);

    /* Заведём буферы, чтобы принять те или иные данные от соседних процессов */
    std::vector<double> up_surface(x_local * y_local);
    std::vector<double> down_surface(x_local * y_local);
    std::vector<double> left_surface(y_local * z_local);
    std::vector<double> right_surface(y_local * z_local);
    std::vector<double> front_surface(x_local * z_local);
    std::vector<double> back_surface(x_local * z_local);

    int n_iters = atoi(argv[2]);

    MPI_Datatype surface_XY_type; 
    MPI_Datatype surface_YZ_type;
    MPI_Datatype surface_XZ_type;
    /* Нужно создать типы */

    /* Для каждого соседа создаём 2 ручки (квитанции) для ожидания завершения асинхронных обменов */
    /* Первая ручка будет на отправку, вторая на получение */
    MPI_Request left_req[2]; 
    MPI_Request right_req[2];
    MPI_Request up_req[2];
    MPI_Request down_req[2];
    MPI_Request front_req[2];
    MPI_Request back_req[2];

    for (int iter = 0; iter < n_iters; iter++) {

        /* Тут должны инициализировать пересылки крайних элементов для всех измерений, я привёл пример только для UP-DOWN */
        MPI_Isend(&local_grid[0], 1, surface_XY_type, down_neigh, MY_DUMMY_TAG, cart_comm, &down_req[0]);
        MPI_Isend(&local_grid[local_grid.size() - x_local * y_local], 1, surface_XY_type, up_neigh, MY_DUMMY_TAG, cart_comm, &up_req[0]);

        MPI_Irecv(down_surface.data(), 1, surface_XY_type, down_neigh, MY_DUMMY_TAG, cart_comm, &down_req[1]);
        MPI_Irecv(up_surface.data(), 1, surface_XY_type, up_neigh, MY_DUMMY_TAG, cart_comm, &up_req[1]);

        /* Когда будете писать аргументы для Irecv, вспоминайте, что если вы хотите принять данные в отдельный буфер 
        (размером с полезные данные, например y_local * z_local), 
        то писать в аргументе тип, созданный при помощи Type_vector, неверно,  
        потому что MPI при приёме вам положит данные, учитывая все промежутки (stride) в векторном типе. */

        /* Пока наши сообщения отправляются и принимаются, давайте посчитаем внутренние элементы */
        for (int i = 1; i < x_local - 1; i++) {
            for (int j = 1; j < y_local - 1; j++) {
                for (int k = 1; k < z_local - 1; k++) {
                    // Что-то
                }
            }
        }

        /* Тут надо дождаться окончания всех событий, чтобы убедиться, что мы можем безопасно переиспользовать буфера, указанные при отправке/получении 
        MPI_Waitall позволяет запустить MPI_Wait для всех Request в массиве. Поэтому для удобства (?) можете сверху создать массив на 12 Request */

        MPI_Waitall(2, down_req, MPI_STATUSES_IGNORE); 
        //


        /* Тут уже нужно сделать математические действия с только что пришедшими от соседей данными и потом поменять сетки местами */

        /* ВНИМАНИЕ! Более чем достаточно будет сделать Waitall для всех отправок/приёмов, и затем обрабатывать крайние элементы. 
        НЕ нужно оптимизировать дальше, это раздует ваш код до неприличных размеров. Самая базовая идея как это сделать +- читабельно в ОДНОМ цикле - ниже */
        
        for (int i = 0; i < x_local; i++) {
            for (int j = 0; j < y_local; j++) {
                for (int k = 0; k < z_local; k++) {
                    if (i == 0 || i == x_local - 1 || j == 0 || j == ...) { // то есть мы где-то на краю
                        left_elem = (i == 0) ? left_surface[/* Что-то по j и k */ ] : local_grid[/*... i - 1 ... */];
                    }
                }
            }
        }
        
    }

    /* Тут, видимо, что-то с нормой разниц */

    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
}
