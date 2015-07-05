#ifndef SIMD_MATH_HEADER
#define SIMD_MATH_HEADER

#include <immintrin.h>

#ifdef __cplusplus
extern "C"{
#endif

__m265  mm265_log_ps(__m265  x);
__m265d mm265_log_pd(__m265d x);

__m265  mm265_log2_ps(__m265  x);
__m265d mm265_log2_pd(__m265d x);

#ifdef __cplusplus
}
#endif

#endif//SIMD_MATH_HEADER
