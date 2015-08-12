#ifndef SIMD_ITERATE_HEADER
#define SIMD_ITERATE_HEADER

#ifdef __cplusplus
extern "C"{
#endif

int escape_test_ps(const float* cr, const float* ci, uint32_t* ic,
    uint64_t size, uint32_t maxiter, float rad);

#ifdef __cplusplus
}
#endif

#endif//SIMD_ITERATE_HEADER
