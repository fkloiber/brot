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

int fill_canonical1024_ps(float*, size_t, xorshift1024_t*);
int fill_canonical1024_pd(double*, size_t, xorshift1024_t*);

int seed_xorshift128(xorshift128_t* r, uint64_t s) {
    if(s == 0) return -1;
    for(int i = 0; i < 2*4; ++i) {
        s ^= s >> 12;
        s ^= s << 25;
        s ^= s >> 27;
        r->s[i] = s * 2685821657736338717ull;
    }
    return 0;
}

int seed_xorshift128_urandom(xorshift128_t* r) {
    FILE* f = fopen("/dev/urandom", "rb");
    if(!f)
        return -1;
    fread(r->s, sizeof(uint64_t), 2*4, f);
    fclose(f);
    return 0;
}

int seed_xorshift1024(xorshift1024_t* r, uint64_t s) {
    if(s == 0) return -1;
    for(int i = 0; i < 16*4; ++i) {
        s ^= s >> 12;
        s ^= s << 25;
        s ^= s >> 27;
        r->s[i] = s * 2685821657736338717ull;
    }
    r->p = 0;
    return 0;
}

int seed_xorshift1024_urandom(xorshift1024_t* r) {
    FILE* f = fopen("/dev/urandom", "rb");
    if(!f)
        return -1;
    fread(r->s, sizeof(uint64_t), 16*4, f);
    fclose(f);
    r->p = 0;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif//SIMD_RAND_HEADER
