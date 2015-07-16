#include <cmath>
#include <limits>
#include <random>
#include <chrono>

#include <gtest/gtest.h>
#include <brot/math.h>

using hrc = std::chrono::high_resolution_clock;

union ymm
{
    __m256d pd;
    __m256 ps;
    double dbl[4];
    float flt[8];
    uint64_t i64[4];
    uint32_t i32[8];
};

union flt
{
    float v;
    uint32_t i;
};

union dbl
{
    double v;
    uint64_t i;
};

constexpr size_t DBL_N = 100000;
constexpr size_t FLT_N = 100000;

constexpr flt f_min = {.v=std::numeric_limits<float>::min()};
constexpr flt f_max = {.v=std::numeric_limits<float>::max()};

constexpr dbl d_min = {.v=std::numeric_limits<double>::min()};
constexpr dbl d_max = {.v=std::numeric_limits<double>::max()};

TEST(mm256_log_pd, PowersOfTwo) {
    ymm x;
    x.dbl[0] = 1.0;
    x.dbl[1] = 2.0;
    x.dbl[2] = 4.0;
    x.dbl[3] = 8.0;

    x.pd = mm256_log_pd(x.pd);

    EXPECT_DOUBLE_EQ(0.0, x.dbl[0]);
    EXPECT_DOUBLE_EQ(std::log(2.0), x.dbl[1]);
    EXPECT_DOUBLE_EQ(std::log(4.0), x.dbl[2]);
    EXPECT_DOUBLE_EQ(std::log(8.0), x.dbl[3]);
}

TEST(mm256_log_pd, SpecialCases) {
    ymm x;
    x.dbl[0] = std::numeric_limits<double>::infinity();
    x.dbl[1] = -1.0;
    x.dbl[2] = 0.0;
    x.dbl[3] = std::numeric_limits<double>::min()/2.0;

    x.pd = mm256_log_pd(x.pd);

    EXPECT_DOUBLE_EQ( std::numeric_limits<double>::infinity(),  x.dbl[0]);
    EXPECT_TRUE(std::isnan(x.dbl[1]));
    EXPECT_DOUBLE_EQ(-std::numeric_limits<double>::infinity(),  x.dbl[2]);
    EXPECT_DOUBLE_EQ(-std::numeric_limits<double>::infinity(),  x.dbl[3]);
}

TEST(mm256_log_pd, RandomCases) {
    ymm x, y;
    std::mt19937 gen(hrc::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist(d_min.i, d_max.i);

    for(size_t i = 0; i < DBL_N; ++i) {
        dbl d;
        d.i = dist(gen);
        x.dbl[0] = d.v;
        d.i = dist(gen);
        x.dbl[1] = d.v;
        d.i = dist(gen);
        x.dbl[2] = d.v;
        d.i = dist(gen);
        x.dbl[3] = d.v;

        y.pd = mm256_log_pd(x.pd);

        EXPECT_DOUBLE_EQ(std::log(x.dbl[0]), y.dbl[0]);
        EXPECT_DOUBLE_EQ(std::log(x.dbl[1]), y.dbl[1]);
        EXPECT_DOUBLE_EQ(std::log(x.dbl[2]), y.dbl[2]);
        EXPECT_DOUBLE_EQ(std::log(x.dbl[3]), y.dbl[3]);
    }
}

TEST(mm256_log_pd, RelativeError) {
    ymm x, y;
    std::mt19937 gen(hrc::now().time_since_epoch().count()+1);
    std::uniform_int_distribution<uint64_t> dist(d_min.i, d_max.i);

    double maxrelerr = 0x1.0p-51;

    for(size_t i = 0; i < DBL_N; ++i) {
        dbl d;
        d.i = dist(gen);
        x.dbl[0] = d.v;
        d.i = dist(gen);
        x.dbl[1] = d.v;
        d.i = dist(gen);
        x.dbl[2] = d.v;
        d.i = dist(gen);
        x.dbl[3] = d.v;

        y.pd = mm256_log_pd(x.pd);

        if(y.dbl[0] != 0.0)
            EXPECT_LE(std::abs(1.0 - std::log(x.dbl[0]) / y.dbl[0]), maxrelerr);
        if(y.dbl[1] != 0.0)
            EXPECT_LE(std::abs(1.0 - std::log(x.dbl[1]) / y.dbl[1]), maxrelerr);
        if(y.dbl[2] != 0.0)
            EXPECT_LE(std::abs(1.0 - std::log(x.dbl[2]) / y.dbl[2]), maxrelerr);
        if(y.dbl[3] != 0.0)
            EXPECT_LE(std::abs(1.0 - std::log(x.dbl[3]) / y.dbl[3]), maxrelerr);
    }
}



TEST(mm256_log_ps, PowersOfTwo) {
    ymm x;
    x.flt[0] = 1.0;
    x.flt[1] = 2.0;
    x.flt[2] = 4.0;
    x.flt[3] = 8.0;

    x.ps = mm256_log_ps(x.ps);

    EXPECT_FLOAT_EQ(0.0, x.flt[0]);
    EXPECT_FLOAT_EQ(std::log(2.0), x.flt[1]);
    EXPECT_FLOAT_EQ(std::log(4.0), x.flt[2]);
    EXPECT_FLOAT_EQ(std::log(8.0), x.flt[3]);
}

TEST(mm256_log_ps, SpecialCases) {
    ymm x;
    x.flt[0] = std::numeric_limits<float>::infinity();
    x.flt[1] = -1.0;
    x.flt[2] = 0.0;
    x.flt[3] = std::numeric_limits<float>::min()/2.0;

    x.ps = mm256_log_ps(x.ps);

    EXPECT_FLOAT_EQ( std::numeric_limits<float>::infinity(),  x.flt[0]);
    EXPECT_TRUE(std::isnan(x.flt[1]));
    EXPECT_FLOAT_EQ(-std::numeric_limits<float>::infinity(),  x.flt[2]);
    EXPECT_FLOAT_EQ(-std::numeric_limits<float>::infinity(),  x.flt[3]);
}

TEST(mm256_log_ps, RandomCases) {
    ymm x, y;
    std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint32_t> dist(f_min.i, f_max.i);

    for(size_t i = 0; i < 100; ++i) {
        for(int j = 0; j < 8; ++j) {
            flt f;
            f.i = dist(gen);
            x.flt[j] = f.v;
            x.flt[j] = std::generate_canonical<float, 32>(gen);
        }

        y.ps = mm256_log_ps(x.ps);

        for(int j = 0; j < 8; ++j)
            EXPECT_FLOAT_EQ(std::log(x.flt[j]), y.flt[j]);
    }
}

TEST(mm256_log_ps, RelativeError) {
    ymm x, y, z;
    std::mt19937 gen(hrc::now().time_since_epoch().count()+1);

    float maxrelerr = 0x1.0p-22f;

    for(size_t i = 0; i < 100; ++i) {
        for(int j = 0; j < 8; ++j)
            x.flt[j] = std::generate_canonical<float, 32>(gen);

        y.ps = mm256_log_ps(x.ps);

        for(int j = 0; j < 8; ++j) {
            z.flt[j] = std::abs(1.0f - std::log(x.flt[j]) / y.flt[j]);
            EXPECT_LE(z.flt[j], maxrelerr);
        }
    }
}

TEST(mm256_log_ps, Exhaustive) {
    union uni{float f; uint32_t i;};
    uni min, max;
    min.f = std::numeric_limits<float>::min();
    max.f = std::numeric_limits<float>::max();
    float maxrelerr = 0x1.0p-22;

    #pragma omp parallel for
    for(uint32_t i = min.i; i <= max.i; i += 8) {
        ymm x, y;
        x.i32[0] = i + 0;
        x.i32[1] = i + 1;
        x.i32[2] = i + 2;
        x.i32[3] = i + 3;
        x.i32[4] = i + 4;
        x.i32[5] = i + 5;
        x.i32[6] = i + 6;
        x.i32[7] = i + 7;
        y.ps = mm256_log_ps(x.ps);
        if(y.flt[0] != 0.0f)
            EXPECT_LE(std::abs(1.0f - std::log(x.flt[0]) / y.flt[0]), maxrelerr);
        if(y.flt[1] != 0.0f)
            EXPECT_LE(std::abs(1.0f - std::log(x.flt[1]) / y.flt[1]), maxrelerr);
        if(y.flt[2] != 0.0f)
            EXPECT_LE(std::abs(1.0f - std::log(x.flt[2]) / y.flt[2]), maxrelerr);
        if(y.flt[3] != 0.0f)
            EXPECT_LE(std::abs(1.0f - std::log(x.flt[3]) / y.flt[3]), maxrelerr);
        if(y.flt[4] != 0.0f)
            EXPECT_LE(std::abs(1.0f - std::log(x.flt[4]) / y.flt[4]), maxrelerr);
        if(y.flt[5] != 0.0f)
            EXPECT_LE(std::abs(1.0f - std::log(x.flt[5]) / y.flt[5]), maxrelerr);
        if(y.flt[6] != 0.0f)
            EXPECT_LE(std::abs(1.0f - std::log(x.flt[6]) / y.flt[6]), maxrelerr);
        if(y.flt[7] != 0.0f)
            EXPECT_LE(std::abs(1.0f - std::log(x.flt[7]) / y.flt[7]), maxrelerr);
    }
}
