#include <cstdio>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>

#include <brot/math.h>

using hrc = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;
using sd = std::chrono::duration<double>;

int main()
{
    constexpr size_t N = 100 << 20;
    constexpr size_t N_pd = N/sizeof(double);
    constexpr size_t N_ps = N/sizeof(float);

    printf("Comparing std::log to mm256_log_pd/mm256_log_ps with %zuMiB input data\n", N>>20);

    double * data_pd = (double*)_mm_malloc(N_pd*sizeof(double), 32);
    float  * data_ps = (float*) _mm_malloc(N_ps*sizeof(float),  32);

    double * outl_pd = (double*)_mm_malloc(N_pd*sizeof(double), 32);
    float  * outl_ps = (float*) _mm_malloc(N_ps*sizeof(float),  32);

    double * outa_pd = (double*)_mm_malloc(N_pd*sizeof(double), 32);
    float  * outa_ps = (float*) _mm_malloc(N_ps*sizeof(float),  32);

    double * err_pd = (double*)_mm_malloc(N_pd*sizeof(double), 32);
    float  * err_ps = (float*) _mm_malloc(N_ps*sizeof(float),  32);

    size_t * idx_pd = (size_t*)malloc(N_pd*sizeof(size_t));
    size_t * idx_ps = (size_t*)malloc(N_ps*sizeof(size_t));

    if(data_pd == nullptr || data_ps == nullptr ||
       outl_pd == nullptr || outl_ps == nullptr ||
       outa_pd == nullptr || outa_ps == nullptr ||
        err_pd == nullptr ||  err_ps == nullptr) {
        return 1;
    }

    auto rng = std::mt19937(hrc::now().time_since_epoch().count());

    printf("Filling double input data... ");
    fflush(stdout);
    for(size_t i = 0; i < N_pd; ++i) {
        data_pd[i] = /*100.0  */ std::generate_canonical<double, 64>(rng);
    }
    printf("done\n");

    printf("Filling float input data... ");
    fflush(stdout);
    for(size_t i = 0; i < N_ps; ++i) {
        data_ps[i] = /*100.0f */ std::generate_canonical<float, 32>(rng);
    }
    printf("done\n\n");

    printf("Testing serial run:\n\n");

    printf("Running std::log double... ");
    fflush(stdout);
    auto log_pd_s_time_start = hrc::now();
    for(size_t i = 0; i < N_pd; i += 8) {
        outl_pd[i+0] = std::log(data_pd[i+0]);
        outl_pd[i+1] = std::log(data_pd[i+1]);
        outl_pd[i+2] = std::log(data_pd[i+2]);
        outl_pd[i+3] = std::log(data_pd[i+3]);
        outl_pd[i+4] = std::log(data_pd[i+4]);
        outl_pd[i+5] = std::log(data_pd[i+5]);
        outl_pd[i+6] = std::log(data_pd[i+6]);
        outl_pd[i+7] = std::log(data_pd[i+7]);
    }
    auto log_pd_s_time_end = hrc::now();
    printf("done in %lums\n", std::chrono::duration_cast<ms>(log_pd_s_time_end - log_pd_s_time_start).count());

    printf("Running mm256_log_pd... ");
    fflush(stdout);
    auto avx_pd_s_time_start = hrc::now();
    for(size_t i = 0; i < N_pd; i += 8) {
        _mm256_store_pd(outa_pd+i+0, mm256_log_pd(_mm256_load_pd(data_pd+i+0)));
        _mm256_store_pd(outa_pd+i+4, mm256_log_pd(_mm256_load_pd(data_pd+i+4)));
    }
    auto avx_pd_s_time_end = hrc::now();
    printf("done in %lums\n", std::chrono::duration_cast<ms>(avx_pd_s_time_end - avx_pd_s_time_start).count());

    printf("Running std::log float... ");
    fflush(stdout);
    auto log_ps_s_time_start = hrc::now();
    for(size_t i = 0; i < N_ps; i += 16) {
        outl_ps[i+ 0] = std::log(data_ps[i+ 0]);
        outl_ps[i+ 1] = std::log(data_ps[i+ 1]);
        outl_ps[i+ 2] = std::log(data_ps[i+ 2]);
        outl_ps[i+ 3] = std::log(data_ps[i+ 3]);
        outl_ps[i+ 4] = std::log(data_ps[i+ 4]);
        outl_ps[i+ 5] = std::log(data_ps[i+ 5]);
        outl_ps[i+ 6] = std::log(data_ps[i+ 6]);
        outl_ps[i+ 7] = std::log(data_ps[i+ 7]);
        outl_ps[i+ 8] = std::log(data_ps[i+ 8]);
        outl_ps[i+ 9] = std::log(data_ps[i+ 9]);
        outl_ps[i+10] = std::log(data_ps[i+10]);
        outl_ps[i+11] = std::log(data_ps[i+11]);
        outl_ps[i+12] = std::log(data_ps[i+12]);
        outl_ps[i+13] = std::log(data_ps[i+13]);
        outl_ps[i+14] = std::log(data_ps[i+14]);
        outl_ps[i+15] = std::log(data_ps[i+15]);
    }
    auto log_ps_s_time_end = hrc::now();
    printf("done in %lums\n", std::chrono::duration_cast<ms>(log_ps_s_time_end - log_ps_s_time_start).count());

    printf("Running mm256_log_ps... ");
    fflush(stdout);
    auto avx_ps_s_time_start = hrc::now();
    for(size_t i = 0; i < N_ps; i += 16) {
        _mm256_store_ps(outa_ps+i+0, mm256_log_ps(_mm256_load_ps(data_ps+i+0)));
        _mm256_store_ps(outa_ps+i+8, mm256_log_ps(_mm256_load_ps(data_ps+i+8)));
    }
    auto avx_ps_s_time_end = hrc::now();
    printf("done in %lums\n", std::chrono::duration_cast<ms>(avx_ps_s_time_end - avx_ps_s_time_start).count());


    printf("\n\nTesting parallel run:\n\n");

    printf("Running std::log double... ");
    fflush(stdout);
    auto log_pd_p_time_start = hrc::now();
    #pragma omp parallel for
    for(size_t i = 0; i < N_pd; i += 8) {
        outl_pd[i+0] = std::log(data_pd[i+0]);
        outl_pd[i+1] = std::log(data_pd[i+1]);
        outl_pd[i+2] = std::log(data_pd[i+2]);
        outl_pd[i+3] = std::log(data_pd[i+3]);
        outl_pd[i+4] = std::log(data_pd[i+4]);
        outl_pd[i+5] = std::log(data_pd[i+5]);
        outl_pd[i+6] = std::log(data_pd[i+6]);
        outl_pd[i+7] = std::log(data_pd[i+7]);
    }
    auto log_pd_p_time_end = hrc::now();
    printf("done in %lums\n", std::chrono::duration_cast<ms>(log_pd_p_time_end - log_pd_p_time_start).count());

    printf("Running mm256_log_pd... ");
    fflush(stdout);
    auto avx_pd_p_time_start = hrc::now();
    #pragma omp parallel for
    for(size_t i = 0; i < N_pd; i += 8) {
        _mm256_store_pd(outa_pd+i+0, mm256_log_pd(_mm256_load_pd(data_pd+i+0)));
        _mm256_store_pd(outa_pd+i+4, mm256_log_pd(_mm256_load_pd(data_pd+i+4)));
    }
    auto avx_pd_p_time_end = hrc::now();
    printf("done in %lums\n", std::chrono::duration_cast<ms>(avx_pd_p_time_end - avx_pd_p_time_start).count());

    printf("Running std::log float... ");
    fflush(stdout);
    auto log_ps_p_time_start = hrc::now();
    #pragma omp parallel for
    for(size_t i = 0; i < N_ps; i += 16) {
        outl_ps[i+ 0] = std::log(data_ps[i+ 0]);
        outl_ps[i+ 1] = std::log(data_ps[i+ 1]);
        outl_ps[i+ 2] = std::log(data_ps[i+ 2]);
        outl_ps[i+ 3] = std::log(data_ps[i+ 3]);
        outl_ps[i+ 4] = std::log(data_ps[i+ 4]);
        outl_ps[i+ 5] = std::log(data_ps[i+ 5]);
        outl_ps[i+ 6] = std::log(data_ps[i+ 6]);
        outl_ps[i+ 7] = std::log(data_ps[i+ 7]);
        outl_ps[i+ 8] = std::log(data_ps[i+ 8]);
        outl_ps[i+ 9] = std::log(data_ps[i+ 9]);
        outl_ps[i+10] = std::log(data_ps[i+10]);
        outl_ps[i+11] = std::log(data_ps[i+11]);
        outl_ps[i+12] = std::log(data_ps[i+12]);
        outl_ps[i+13] = std::log(data_ps[i+13]);
        outl_ps[i+14] = std::log(data_ps[i+14]);
        outl_ps[i+15] = std::log(data_ps[i+15]);
    }
    auto log_ps_p_time_end = hrc::now();
    printf("done in %lums\n", std::chrono::duration_cast<ms>(log_ps_p_time_end - log_ps_p_time_start).count());

    printf("Running mm256_log_ps... ");
    fflush(stdout);
    auto avx_ps_p_time_start = hrc::now();
    #pragma omp parallel for
    for(size_t i = 0; i < N_ps; i += 16) {
        _mm256_store_ps(outa_ps+i+0, mm256_log_ps(_mm256_load_ps(data_ps+i+0)));
        _mm256_store_ps(outa_ps+i+8, mm256_log_ps(_mm256_load_ps(data_ps+i+8)));
    }
    auto avx_ps_p_time_end = hrc::now();
    printf("done in %lums\n", std::chrono::duration_cast<ms>(avx_ps_p_time_end - avx_ps_p_time_start).count());

    printf("\nCalculating errors... ");
    fflush(stdout);
    #pragma omp parallel for
    for(size_t i = 0; i < N_pd; ++i) {
        err_pd[i] = std::abs(1.0 - outa_pd[i]/outl_pd[i]);
    }
    #pragma omp parallel for
    for(size_t i = 0; i < N_ps; ++i) {
        err_ps[i] = std::abs(1.0 - outa_ps[i]/outl_ps[i]);
    }
    #pragma omp parallel for
    for(size_t i = 0; i < N_pd; ++i) {
        idx_pd[i] = i;
    }
    #pragma omp parallel for
    for(size_t i = 0; i < N_ps; ++i) {
        idx_ps[i] = i;
    }

    std::sort(idx_pd, idx_pd+N_pd, [&](size_t a, size_t b){ return err_pd[a] < err_pd[b]; });
    std::sort(idx_ps, idx_ps+N_ps, [&](size_t a, size_t b){ return err_ps[a] < err_ps[b]; });
    printf("done\n");

    printf("\n\nSummary:\n");
    double lsd_s = std::chrono::duration_cast<sd>(log_pd_s_time_end - log_pd_s_time_start).count();
    double asd_s = std::chrono::duration_cast<sd>(avx_pd_s_time_end - avx_pd_s_time_start).count();
    double lss_s = std::chrono::duration_cast<sd>(log_ps_s_time_end - log_ps_s_time_start).count();
    double ass_s = std::chrono::duration_cast<sd>(avx_ps_s_time_end - avx_ps_s_time_start).count();
    double lpd_s = std::chrono::duration_cast<sd>(log_pd_p_time_end - log_pd_p_time_start).count();
    double apd_s = std::chrono::duration_cast<sd>(avx_pd_p_time_end - avx_pd_p_time_start).count();
    double lps_s = std::chrono::duration_cast<sd>(log_ps_p_time_end - log_ps_p_time_start).count();
    double aps_s = std::chrono::duration_cast<sd>(avx_ps_p_time_end - avx_ps_p_time_start).count();

    printf(" Algorithm    | Data Type | parallel | time      | speed      | min rel err  | max rel err  | 90%% rel err\n");
    printf("-----------------------------------------------------------------------------------------------------------\n");
    printf(" std::log     | double    |  false   | %6.1f ms | %4.2f GiB/s\n", 1000.0 * lsd_s, N / lsd_s / (1<<30));
    printf(" mm256_log_pd | double    |  false   | %6.1f ms | %4.2f GiB/s | %e | %e | %e\n", 1000.0 * asd_s, N / asd_s / (1<<30), err_pd[idx_pd[0]], err_pd[idx_pd[N_pd-1]], err_pd[idx_pd[90*N_pd/100]]);
    printf(" std::log     | float     |  false   | %6.1f ms | %4.2f GiB/s\n", 1000.0 * lss_s, N / lss_s / (1<<30));
    printf(" mm256_log_ps | float     |  false   | %6.1f ms | %4.2f GiB/s | %e | %e | %e\n", 1000.0 * ass_s, N / ass_s / (1<<30), err_ps[idx_ps[0]], err_ps[idx_ps[N_ps-1]], err_ps[idx_ps[90*N_ps/100]]);
    printf(" std::log     | double    |  true    | %6.1f ms | %4.2f GiB/s\n", 1000.0 * lpd_s, N / lpd_s / (1<<30));
    printf(" mm256_log_pd | double    |  true    | %6.1f ms | %4.2f GiB/s | %e | %e | %e\n", 1000.0 * apd_s, N / apd_s / (1<<30), err_pd[idx_pd[0]], err_pd[idx_pd[N_pd-1]], err_pd[idx_pd[90*N_pd/100]]);
    printf(" std::log     | float     |  true    | %6.1f ms | %4.2f GiB/s\n", 1000.0 * lps_s, N / lps_s / (1<<30));
    printf(" mm256_log_ps | float     |  true    | %6.1f ms | %4.2f GiB/s | %e | %e | %e\n", 1000.0 * aps_s, N / aps_s / (1<<30), err_ps[idx_ps[0]], err_ps[idx_ps[N_ps-1]], err_ps[idx_ps[90*N_ps/100]]);

    _mm_free(data_pd);
    _mm_free(data_ps);
    _mm_free(outl_pd);
    _mm_free(outl_ps);
    _mm_free(outa_pd);
    _mm_free(outa_ps);
    _mm_free(err_pd);
    _mm_free(err_ps);

    free(idx_pd);
    free(idx_ps);

    return 0;
}
