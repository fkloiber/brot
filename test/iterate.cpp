#include <cstdio>
#include <cstdint>
#include <vector>
#include <random>

#include <gtest/gtest.h>
#include <brot/iterate.h>

TEST(escape_test_ps, error_codes)
{
    std::vector<float> cr(8);
    std::vector<float> ci(8);
    std::vector<uint32_t> ic(8);

    EXPECT_EQ(escape_test_ps(cr.data(), ci.data(), ic.data(), 7, 100, 2.0f), -4);
    EXPECT_EQ(escape_test_ps(cr.data(), ci.data(), nullptr, 8, 100, 2.0f), -3);
    EXPECT_EQ(escape_test_ps(cr.data(), nullptr, ic.data(), 8, 100, 2.0f), -2);
    EXPECT_EQ(escape_test_ps(nullptr, ci.data(), ic.data(), 8, 100, 2.0f), -1);
    EXPECT_EQ(escape_test_ps(cr.data(), ci.data(), ic.data(), 8, 100, 2.0f),  0);
}

TEST(escape_test_ps, iteration_count)
{
    std::vector<float> cr(8);
    std::vector<float> ci(8);
    std::vector<uint32_t> i(8);

    cr[0] =  0.0f;
    ci[0] =  0.0f;
    cr[1] =  2.0f;
    ci[1] =  2.5f;
    cr[2] =  1.0f;
    ci[2] =  0.0f;
    cr[3] = -1.0f;
    ci[3] =  0.0f;
    cr[4] =  0.0f;
    ci[4] =  1.0f;
    cr[5] =  0.0f;
    ci[5] = -1.0f;
    cr[6] = -0.75f;
    ci[6] =  0.1f;
    cr[7] =  0.275f;
    ci[7] =  0.0f;

    escape_test_ps(cr.data(), ci.data(), i.data(), 8, 100, 2.0f);
    EXPECT_EQ(i[0], 100u);
    EXPECT_EQ(i[1],   0u);
    EXPECT_EQ(i[2],   2u);
    EXPECT_EQ(i[3], 100u);
    EXPECT_EQ(i[4], 100u);
    EXPECT_EQ(i[5], 100u);
    EXPECT_EQ(i[6],  32u);
    EXPECT_EQ(i[7],  17u);
}

TEST(bulb_test_ps, test) {
    const uint32_t N = 1024;
    std::vector<float> cr(N);
    std::vector<float> ci(N);
    std::vector<uint32_t> is(N);
    std::vector<uint32_t> iv(N);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);

    for(float& f : cr) f = dist(rng);
    for(float& f : ci) f = dist(rng);

    for(uint32_t i = 0; i < N; ++i) {
        float xm = cr[i] - 0.25f;
        float xp = cr[i] + 1.0f;
        float y2 = ci[i]*ci[i];
        float q = xm*xm + y2;
        if(q*(q+xm) < 0.25f*y2 || xp*xp + y2 < 0.0625f)
            is[i] = -1;
    }
    bulb_test_ps(cr.data(), ci.data(), iv.data(), N);
    for(uint32_t i = 0; i < N; ++i) {
        EXPECT_EQ(is[i], iv[i]);
    }
}

TEST(bulb_test_pd, test) {
    const uint64_t N = 1024;
    std::vector<double> cr(N);
    std::vector<double> ci(N);
    std::vector<uint64_t> is(N);
    std::vector<uint64_t> iv(N);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<double> dist(-2.0f, 2.0f);

    for(double& f : cr) f = dist(rng);
    for(double& f : ci) f = dist(rng);

    for(uint64_t i = 0; i < N; ++i) {
        double xm = cr[i] - 0.25;
        double xp = cr[i] + 1.0;
        double y2 = ci[i]*ci[i];
        double q = xm*xm + y2;
        if(q*(q+xm) < 0.25*y2 || xp*xp + y2 < 0.0625)
            is[i] = -1;
    }
    bulb_test_pd(cr.data(), ci.data(), iv.data(), N);
    for(uint64_t i = 0; i < N; ++i) {
        EXPECT_EQ(is[i], iv[i]);
    }
}

TEST(write_orbits_pd, error_codes) {
    std::vector<double> cr(4);
    std::vector<double> ci(4);
    std::vector<uint32_t> img(100*100);

    EXPECT_EQ(write_orbits_pd(nullptr, ci.data(), 4, 1000, -2.0, 2.0, -2.0, 2.0, img.data(), 100, 100), -1);
    EXPECT_EQ(write_orbits_pd(cr.data(), nullptr, 4, 1000, -2.0, 2.0, -2.0, 2.0, img.data(), 100, 100), -2);
    EXPECT_EQ(write_orbits_pd(cr.data(), ci.data(), 4, 1000, -2.0, 2.0, -2.0, 2.0, nullptr, 100, 100), -3);
    EXPECT_EQ(write_orbits_pd(cr.data(), ci.data(), 3, 1000, -2.0, 2.0, -2.0, 2.0, img.data(), 100, 100), -4);
    EXPECT_GE(write_orbits_pd(cr.data(), ci.data(), 4, 1000, -2.0, 2.0, -2.0, 2.0, img.data(), 100, 100), 0);
}

TEST(write_orbits_pd, return_value) {
    constexpr size_t N=128;
    constexpr size_t w=100, h=100;
    constexpr size_t iter=1000;
    std::vector<double> cr(N);
    std::vector<double> ci(N);
    std::vector<uint32_t> img(w*h);

    int64_t ret = write_orbits_pd(cr.data(), ci.data(), N, iter, -2.0, 2.0, -2.0, 2.0, img.data(), w, h);
    for(size_t i = 0; i < h/2; ++i) {
    for(size_t j = 0; j < w; ++j) {
        EXPECT_EQ(img[i*w+j], 0u);
    }
    }
    for(size_t j = 0; j < w/2; ++j) {
        EXPECT_EQ(img[w*h/2+j], 0u);
    }
    for(size_t j = w/2+1; j < w; ++j) {
        EXPECT_EQ(img[w*h/2+j], 0u);
    }
    for(size_t i = h/2+1; i < h; ++i) {
    for(size_t j = 0; j < w; ++j) {
        EXPECT_EQ(img[i*w+j], 0u);
    }
    }
    EXPECT_EQ(img[w*h/2+w/2], N*iter);
    EXPECT_EQ(ret, N*iter);
}
