// gcc-14 -O3 -fno-builtin-memcpy -fno-builtin-memmove -march=native -mavx2 -mfma -fopt-info-vec-missed --save-temps -ffp-contract=fast 05_vec_hello.c
#include <stdio.h>
#include <immintrin.h>

int main() {

	__m256d a = _mm256_set_pd(1.0, 2.0, 3.0, 4.0);

	__m256d b = a;

	double output[4];

	__m256d c = _mm256_add_pd(a, b);

	_mm256_storeu_pd (output, c);

	printf("%f %f %f %f\n",
	output[0], output[1], output[2], output[3]);
	return 0;
}
