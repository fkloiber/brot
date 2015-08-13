#ifndef SIMD_ITERATE_HEADER
#define SIMD_ITERATE_HEADER

#ifdef __cplusplus
extern "C"{
#endif

int escape_test_ps(const float* cr, const float* ci, uint32_t* ic,
    uint64_t size, uint32_t maxiter, float rad);
int escape_test_pd(const double* cr, const double* ci, uint64_t* ic,
    uint64_t size, uint64_t maxiter, double rad);

int bulb_test_ps(const float* cr, const float* ci, uint32_t* im, uint64_t size);
int bulb_test_pd(const double* cr, const double* ci, uint64_t* im, uint64_t size);

int write_orbits_ps(const float* cr, const float* ci, uint64_t size,
    uint32_t maxiter, float minr, float maxr, float mini, float maxi,
    uint32_t* img, uint32_t width, uint32_t height);
int write_orbits_pd(const double* cr, const double* ci, uint64_t size,
    uint64_t maxiter, double minr, double maxr, double mini, double maxi,
    uint64_t* img, uint64_t width, uint64_t height);

#ifdef __cplusplus
}
#endif

#endif//SIMD_ITERATE_HEADER
