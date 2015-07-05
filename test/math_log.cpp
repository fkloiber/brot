#include <cmath>
#include <limits>
#include <random>
#include <chrono>

#include <gtest/gtest.h>
#include <brot/math.h>

union _pd
{
    __m256d ymm;
    double flt[4];
    uint64_t i[4];
};

TEST(mm256_log_pd, PowersOfTwo) {
    _pd x;
    x.flt[0] = 1.0;
    x.flt[1] = 2.0;
    x.flt[2] = 4.0;
    x.flt[3] = 8.0;

    x.ymm = mm256_log_pd(x.ymm);

    EXPECT_EQ(0.0, x.flt[0]);
    EXPECT_EQ(std::log(2.0), x.flt[1]);
    EXPECT_EQ(std::log(4.0), x.flt[2]);
    EXPECT_EQ(std::log(8.0), x.flt[3]);
}

TEST(mm256_log_pd, SpecialCases) {
    _pd x;
    x.flt[0] = std::numeric_limits<double>::infinity();
    x.flt[1] = -1.0;
    x.flt[2] = 0.0;
    x.flt[3] = std::numeric_limits<double>::min()/2.0;

    x.ymm = mm256_log_pd(x.ymm);

    EXPECT_EQ( std::numeric_limits<double>::infinity(),  x.flt[0]);
    EXPECT_TRUE(std::isnan(x.flt[1]));
    EXPECT_EQ(-std::numeric_limits<double>::infinity(),  x.flt[2]);
    EXPECT_EQ(-std::numeric_limits<double>::infinity(),  x.flt[3]);
}

TEST(mm256_log_pd, RandomCases) {
    _pd x, y;
    std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    for(int i = 0; i < 20; ++i) {
        for(int j = 0; j < 4; ++j)
            x.flt[j] = std::generate_canonical<double, 64>(gen);

        y.ymm = mm256_log_pd(x.ymm);

        for(int j = 0; j < 4; ++j)
            EXPECT_DOUBLE_EQ(std::log(x.flt[j]), y.flt[j]);
    }
}
