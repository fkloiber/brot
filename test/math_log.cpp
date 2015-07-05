#include <cmath>
#include <limits>
#include <random>
#include <chrono>

#include <gtest/gtest.h>
#include <brot/math.h>

union ymm
{
    __m256d pd;
    __m256 ps;
    double dbl[4];
    float flt[4];
    uint64_t i[4];
};

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
    std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    for(int i = 0; i < 20; ++i) {
        for(int j = 0; j < 4; ++j)
            x.dbl[j] = std::generate_canonical<double, 64>(gen);

        y.pd = mm256_log_pd(x.pd);

        for(int j = 0; j < 4; ++j)
            EXPECT_DOUBLE_EQ(std::log(x.dbl[j]), y.dbl[j]);
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

    for(int i = 0; i < 20; ++i) {
        for(int j = 0; j < 4; ++j)
            x.flt[j] = std::generate_canonical<float, 32>(gen);

        y.ps = mm256_log_ps(x.ps);

        for(int j = 0; j < 4; ++j)
            EXPECT_FLOAT_EQ(std::log(x.flt[j]), y.flt[j]);
    }
}
