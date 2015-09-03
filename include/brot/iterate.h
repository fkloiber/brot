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

int32_t write_orbits_ps(const float* cr, const float* ci, uint64_t size,
    uint32_t maxiter, float min_r, float max_r, float min_i, float max_i,
    uint32_t* img, uint32_t width, uint32_t height);
int64_t write_orbits_pd(const double* cr, const double* ci, uint64_t size,
    uint64_t maxiter, double min_r, double max_r, double min_i, double max_i,
    uint32_t* img, uint64_t width, uint64_t height);

#ifdef __cplusplus
}

inline int escape_test(const float* cr, const float* ci, uint32_t* ic,
    uint64_t size, uint32_t maxiter, float rad)
{
    return escape_test_ps(cr,ci,ic,size,maxiter,rad);
}

inline int escape_test(const double* cr, const double* ci, uint64_t* ic,
    uint64_t size, uint64_t maxiter, double rad)
{
    return escape_test_pd(cr,ci,ic,size,maxiter,rad);
}

inline int bulb_test(const float* cr, const float* ci, uint32_t* im, uint64_t size)
{
    return bulb_test_ps(cr,ci,im,size);
}

inline int bulb_test(const double* cr, const double* ci, uint64_t* im, uint64_t size)
{
    return bulb_test_pd(cr,ci,im,size);
}

inline int32_t write_orbits(const float* cr, const float* ci, uint64_t size,
    uint32_t maxiter, float min_r, float max_r, float min_i, float max_i,
    uint32_t* img, uint32_t width, uint32_t height)
{
    return write_orbits_ps(cr,ci,size,maxiter,min_r,max_r,min_i,max_i,img,width,height);
}

inline int64_t write_orbits(const double* cr, const double* ci, uint64_t size,
    uint64_t maxiter, double min_r, double max_r, double min_i, double max_i,
    uint32_t* img, uint64_t width, uint64_t height)
{
    return write_orbits_pd(cr,ci,size,maxiter,min_r,max_r,min_i,max_i,img,width,height);
}

#endif

#endif//SIMD_ITERATE_HEADER
