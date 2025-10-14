// g++-14 -O3 -fno-builtin-memcpy -fno-builtin-memmove -march=native -mavx512f -mfma -fopt-info-vec-missed --save-temps -ffp-contract=fast 08_argmin.cpp

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <iostream>
#include <chrono>
#include <climits>

const int NREPS = 500000;

int argmin(const int *a, int n) {
    int i, k = 0;
    for (i = 0; i < n; i++)
        if (a[i] < a[k]) {
            k = i;
        }
    return k;
}


int argmin_vectorized(const int *a, int n) {
    alignas(64) int min_arr[16];
    alignas(64) int idx_arr[16];

   const __m512i lane = _mm512_setr_epi32(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);

    __m512i minv = _mm512_set1_epi32(INT_MAX);
    __m512i idxv = _mm512_set1_epi32(0);

    for (int i = 0; i < n; i += 16) {
        // Загрузили последовательные данные из массива
        __m512i v   = _mm512_load_epi32(a + i);

        // Загрузили текущее значение индекса (прибавляем вектор lane к элементу, являющемуся индексом и кратным 16)          
        __m512i cur = _mm512_add_epi32(_mm512_set1_epi32(i), lane);
        
        // Сравниваем и заполняем маску: 1 если v < minv, иначе 0
        __mmask16 m = _mm512_cmplt_epi32_mask(v, minv);  
        
        // В тех местах, где значение маски 0, оставляем текущий argmin. Где 1 - индекс нужно обновить
        idxv = _mm512_mask_blend_epi32(m, idxv, cur);
        
        //Поэлементно обновляем минимумы 
        minv = _mm512_min_epi32(minv, v);             
    }

    // После того, как нашли вектор из argmin и min, необходимо понять, где в векторе реально argmin и min
    _mm512_store_epi32(min_arr, minv);  
    _mm512_store_epi32(idx_arr, idxv); 

    int k = 0;
    for (int j = 1; j < 16; ++j) {
        if (min_arr[j] < min_arr[k] || (min_arr[j] == min_arr[k] && idx_arr[j] < idx_arr[k])) {
            k = j;
        }
    }
    int res = idx_arr[k];

    // Важно! В этой задаче мы допускаем, что длина входного вектора кратна 16. Если это будет не так, нам нужно ещё найти минимум и позицию минимума в хвосте.
    return res;
}


int main(int argc, char **argv) {
    int i; 
    int reps = NREPS; 
    int mem; 
    mem = atoi(argv[1]);
    mem = 1 << mem;
    srand(time(NULL));

    int* a = static_cast<int*>(_mm_malloc(sizeof(int) * mem, 64));

    for (i = 0; i < mem; ++i) {
        a[i] = rand() % mem;
    }

    volatile int res = argmin(a, mem);
    volatile int res_vec = argmin_vectorized(a, mem);

    if (res != res_vec) {
        printf("Error: position: %d vs %d\n", res, res_vec);
    }

    auto start_slow = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < reps; i++) {
        res = argmin(a, mem);
    }
    auto end_slow = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < reps; i++) {
       res_vec = argmin_vectorized(a, mem);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Standard time is " << std::chrono::duration<double>(end_slow - start_slow).count() << " seconds." << std::endl;
    std::cout << "Optimal time is " << std::chrono::duration<double>(end - start).count() << " seconds." << std::endl;

    _mm_free(a);
}
