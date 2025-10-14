//g++-14 -O3 -ftree-vectorize -fno-builtin-memcpy -fno-builtin-memmove -march=native -mavx512f -mfma -fopt-info-vec-missed --save-temps -ffp-contract=fast 06_vec_matmul.cpp
#include <iostream>
#include <chrono>
#include <x86intrin.h>

static constexpr size_t N = 2048;

void matrix_multiply(const double*, const double*, double*);
void matrix_multiply_standart(const double*, const double*, double*);

void print_matrix(const double* mat) {
    for (int i = 0; i < N * N; i++) {
        std::cout << mat[i] << " ";
    }
}

int main(void)
{
    double* matrixA = static_cast<double*>(_mm_malloc(sizeof(double) * N * N, 64));
    double* matrixB = static_cast<double*>(_mm_malloc(sizeof(double) * N * N, 64));
    double* matrixC = static_cast<double*>(_mm_malloc(sizeof(double) * N * N, 64));
    double* matrixC_slow = static_cast<double*>(_mm_malloc(sizeof(double) * N * N, 64));

    for (int i = 0; i < N * N; i++) {
        matrixA[i] = static_cast<double>(i);
        matrixB[i] = static_cast<double>(-i);
        matrixC[i] = static_cast<double>(0);
        matrixC_slow[i] = static_cast<double>(0);
    }

    auto start_slow = std::chrono::high_resolution_clock::now();
    matrix_multiply_standart(matrixA, matrixB, matrixC_slow);
    auto end_slow = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::high_resolution_clock::now();
    matrix_multiply(matrixA, matrixB, matrixC);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Standard time is " << std::chrono::duration<double>(end_slow - start_slow).count() << " seconds." << std::endl;
    std::cout << "Optimal time is " << std::chrono::duration<double>(end - start).count() << " seconds." << std::endl;

    _mm_free(matrixC);
    _mm_free(matrixC_slow);
    _mm_free(matrixB);
    _mm_free(matrixA);
    
    return 0;
}

void matrix_multiply(const double* A, const double* B, double* C)
{
    for (size_t i = 0; i < N; ++i) {
        for (size_t k = 0; k < N; ++k) {
            __m256d a = _mm256_broadcast_sd(A + i * N + k);
            for (size_t j = 0; j < N; j += 4) {
                __m256d c0 = _mm256_load_pd(C + i * N + j); 
                __m256d b0 = _mm256_load_pd(B + k * N + j); 
                c0 = _mm256_fmadd_pd(a, b0, c0);
                _mm256_store_pd(C + i * N + j, c0);
            }
        }
    }
    // В этом примере допускаем, что размер матрицы кратен 4
}
void matrix_multiply_standart(const double *__restrict A, const double* __restrict B, double* __restrict C)
{
    for (size_t k = 0; k < N; k++) {
        for (size_t i = 0; i < N; i++) {
            double left = A[i * N + k];
            for (size_t j = 0; j < N; j++) {
                C[i * N + j] += left * B[k*N + j];
            }
        }
    }
}