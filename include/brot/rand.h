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

int fill_uniform1024_ps(float*, size_t, float, float, xorshift1024_t*);
int fill_uniform1024_pd(double*, size_t, double, double, xorshift1024_t*);

int seed_xorshift128(xorshift128_t* r, uint64_t s) {
    for(int i = 0; i < 2*4; ++i) {
        s += 0x9E3779B97F4A7C15ull;
        uint64_t z = s;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
        r->s[i] = z ^ (z >> 31);
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
    for(int i = 0; i < 16*4; ++i) {
        s += 0x9E3779B97F4A7C15ull;
        uint64_t z = s;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
        r->s[i] = z ^ (z >> 31);
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
