#ifndef SIMD_RAND_HEADER
#define SIMD_RAND_HEADER

#include <immintrin.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

struct xorshift128_t
{
    uint64_t s[2*4];
};

struct xorshift1024_t
{
    uint64_t s[16*4];
    uint64_t p;
};

__m256i xorshift128_next(xorshift128_t*);
__m256i xorshift1024_next(xorshift1024_t*);

int fill_canonical128_ps(float*, size_t, xorshift128_t*);
int fill_canonical128_pd(double*, size_t, xorshift128_t*);

#ifdef __cplusplus
}
#endif

#endif//SIMD_RAND_HEADER
