// Написать параллельную программу вычисления числа Pi с 
// использованием функций MPI для односторонней передачи. 
// Синхронизацию процессов реализовать с использованием функций
// синхронизации для односторонних передач.

#include "mpi.h"
#include <math.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int done = 0, myid, numprocs, i;
    double PI_math_lib = M_PI;
    double mypi, pi, h, sum, x;
    int n = 10000000;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
   
    /* Посчитали, сколько точек нужно обработать на процесс (допустим целое деление)*/
    int local_n = n/numprocs;
	h   = 1.0 / (double) n;
	sum = 0.0;

	for (i = myid * local_n; i < (myid + 1) * local_n; i++) {
	    x = h * ((double)i + 0.5);
	    sum += 4.0 / (1.0 + x*x);
	}

	mypi = h * sum;

    double result = 4232.0;
    
    /* Пусть процесс 0 соберёт все значения, попутно суммируя */
    // if (myid == 0) {
    //     double part_res;
    //     for (int nb = 1; nb < numprocs; nb++) {
    //         MPI_Recv(&part_res, 1, MPI_DOUBLE, nb, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //         mypi += part_res;
    //     }
    // } else {
    //     MPI_Send(&mypi, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    // }

    double final;

    /* Пусть все получат значение final */
    MPI_Allreduce(&mypi, &final, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    
    /* Уже можем выводить любым процессом */
	if (myid < numprocs) {
	    printf("pi is approximately %.16f, Error is %.16f\n",
		   final, fabs(final - PI_math_lib));
    }
    MPI_Finalize();
    return 0;
}
