#ifndef SIMD_ITERATE_HEADER
#define SIMD_ITERATE_HEADER

#ifdef __cplusplus
extern "C"{
#endif

int test_in_M_ps(const float* points, uint32_t* in, uint64_t size,
    uint32_t maxiter, float rad);

#ifdef __cplusplus
}
#endif

#endif//SIMD_ITERATE_HEADER
