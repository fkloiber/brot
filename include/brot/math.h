#ifndef SIMD_MATH_HEADER
#define SIMD_MATH_HEADER

#include <immintrin.h>

#ifdef __cplusplus
extern "C"{
#endif

__m256  mm256_log_ps(__m256  x);
__m256d mm256_log_pd(__m256d x);

__m256  mm256_log2_ps(__m256  x);
__m256d mm256_log2_pd(__m256d x);

__m256  mm256_log10_ps(__m256  x);
__m256d mm256_log10_pd(__m256d x);

#ifdef __cplusplus
}
#endif

#endif//SIMD_MATH_HEADER
