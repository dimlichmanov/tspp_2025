// Example is taken from K.Vladimirov lectures
// g++-14 -O3 -fno-builtin-memcpy -fno-builtin-memmove -march=native -mavx512f -mfma -fopt-info-vec-missed --save-temps -ffp-contract=fast 07_find.cpp

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <iostream>
#include <chrono>

const int NREPS = 1000000;

int first_equal_zvalue(__m512i src1, __m512i src2);

int find(const int *a, int n, int x) {
  int i;
  for (i = 0; i < n; i++)
    if (a[i] == x)
      return i;
  return -1;
}

int find_vectorized(const int *a, int n, int x) {
    __m512i desire = _mm512_set1_epi32(x);
    int main_size = (n / 16) * 16;

    // ВАЖНО! Обратите внимание, когда мы выгружаем из памяти участок и загружаем его на векторный регистр, 
    // элементы укладываются так, что первый элемент участка *a идёт в младший элемент векторного регистра, 
    // Затем элемент *(a + 1) укладывается во второй по младшинству элемент регистра, и так далее.
    // При выгрузке в память логика сохраняется
    // !! Это объясняет вопрос с маской и тем, почему мы пытаемся найти в ней число нулевых младших битов.

    for (int i = 0; i < main_size; i += 16) {
        __m512i undertest = _mm512_load_epi32(a + i);

        int m = first_equal_zvalue(desire, undertest);
        if (m != -1)
        return i + m;
    }

    // Обрабатываем хвост
    for (int i = main_size; i < n; i += 1)
        if (a[i] == x)
        return i;

    return -1;
}

int first_equal_zvalue(__m512i src1, __m512i src2) {
    __mmask16 mask = _mm512_cmp_epi32_mask(src1, src2, _MM_CMPINT_EQ); //Сравниваем в векторе все 16 значений друг с другом. В маске вернётся 1 на совпавших позициях.
    if (mask != 0)
        return __builtin_ctz(mask);
    return -1;
}

int main(int argc, char **argv) {
    int mem;
    volatile int res;
    volatile int res_vec;
    volatile int reps = NREPS;
    mem = atoi(argv[1]);

    srand(time(NULL));
    mem = 1 << mem;

    int* a = static_cast<int*>(_mm_malloc(sizeof(int) * mem, 64));

    for (int i = 0; i < mem; ++i)
        a[i] = i;

    volatile int target = rand() % mem;

    res = find(a, mem, target);
    res_vec = find_vectorized(a, mem, target);

    if (res != res_vec) {
        printf("Error: position: %d vs %d\n", res, res_vec);
    }

    auto start_slow = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < reps; i++) {
        res = find(a, mem, rand() % mem);
    }
    auto end_slow = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < reps; i++) {
       res_vec = find_vectorized(a, mem, rand() % mem);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Standard time is " << std::chrono::duration<double>(end_slow - start_slow).count() << " seconds." << std::endl;
    std::cout << "Optimal time is " << std::chrono::duration<double>(end - start).count() << " seconds." << std::endl;

    
    free(a);
}