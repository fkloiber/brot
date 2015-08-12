#include <cstdio>
#include <cstdint>
#include <chrono>
#include <vector>

#include <gtest/gtest.h>
#include <brot/rand.h>

union ymm
{
    __m256i si;
    double dbl[4];
    float flt[8];
    uint64_t i64[4];
    uint32_t i32[8];
};

uint64_t xorshift128_state[2];

uint64_t scalar_xorshift128_next() {
    uint64_t s1 = xorshift128_state[0];
    const uint64_t s0 = xorshift128_state[1];
    xorshift128_state[0] = s0;
    s1 ^= s1 << 23;
    xorshift128_state[1] = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26);
    return xorshift128_state[1] + s0;
}

uint64_t xorshift1024_state[16];
uint64_t xorshift1024_idx;

uint64_t scalar_xorshift1024_next() {
    uint64_t s0 = xorshift1024_state[xorshift1024_idx];
    uint64_t s1 = xorshift1024_state[xorshift1024_idx=(xorshift1024_idx+1)&15];
    s1 ^= s1 << 31;
    s1 ^= s1 >> 11;
    s0 ^= s0 >> 30;
    xorshift1024_state[xorshift1024_idx] = s0 ^ s1;
    return xorshift1024_state[xorshift1024_idx] * 1181783497276652981LL;
}




TEST(rand, xorshift128) {
    xorshift128_t state;
    xorshift128_state[0] = 1;
    xorshift128_state[1] = 0;
    for(int i = 0; i < 4; ++i) {
        state.s[i+0] = 1;
        state.s[i+4] = 0;
    }
    for(int i = 0; i < 10000; ++i) {
        const uint64_t scalar = scalar_xorshift128_next();
        ymm vector;
        vector.si = xorshift128_next(&state);
        for(int j = 0; j < 4; ++j)
            EXPECT_EQ(scalar, vector.i64[j]);
    }
}

TEST(rand, xorshift1024) {
    xorshift1024_t state;

    xorshift1024_idx = 0;
    xorshift1024_state[0] = 1;
    for(int i = 1; i < 16; ++i)
        xorshift1024_state[i] = 0;

    state.p = 0;
    for(int i = 0; i < 4; ++i)
        state.s[i] = 1;
    for(int i = 4; i < 16*4; ++i)
        state.s[i] = 0;

    for(int i = 0; i < 10000; ++i) {
        const uint64_t scalar = scalar_xorshift1024_next();
        ymm vector;
        vector.si = xorshift1024_next(&state);
        for(int j = 0; j < 4; ++j)
            EXPECT_EQ(scalar, vector.i64[j]);
    }
}




TEST(fill_canonical128_ps, BufferSizeControl) {
    xorshift128_t state;
    for(int i = 0; i < 4; ++i) {
        state.s[i+0] = 1;
        state.s[i+4] = 0;
    }
    float buffer[16] = {0};
    fill_canonical128_ps(buffer, 14, &state);
    for(int i = 1; i < 4; ++i) {
        EXPECT_EQ(buffer[0], buffer[2*i+0]);
        EXPECT_EQ(buffer[1], buffer[2*i+1]);
    }
    for(int i = 5; i < 7; ++i) {
        EXPECT_EQ(buffer[8], buffer[2*i+0]);
        EXPECT_EQ(buffer[9], buffer[2*i+1]);
    }
    for(int i = 14; i < 16; ++i)
        EXPECT_EQ(0, buffer[i]);
}

TEST(fill_canonical128_ps, Distribution) {
    constexpr size_t N = 1ull<<22;
    xorshift128_t state;
    seed_xorshift128_urandom(&state);

    std::vector<float> buffer(N);

    fill_canonical128_ps(buffer.data(), N, &state);

    for(size_t i = 0; i < N; ++i) {
        EXPECT_GE(buffer[i], 0.0f);
        EXPECT_LT(buffer[i], 1.0f);
    }

    double mean = 0.0;
    for(size_t i = 0; i < N; ++i) {
        mean += buffer[i];
    }
    mean /= N;
    EXPECT_NEAR(mean, 0.5, 0.001);
}

TEST(fill_canonical128_ps, Repeatable) {
    xorshift128_t state1;
    seed_xorshift128_urandom(&state1);

    xorshift128_t state2 = state1;

    std::vector<float> buffer(32);

    fill_canonical128_ps(buffer.data() +  0, 16, &state1);
    fill_canonical128_ps(buffer.data() + 16, 16, &state2);

    for(int i = 0; i < 16; ++i) {
        EXPECT_EQ(buffer[i], buffer[i+16]);
    }
    for(int i = 0; i < 2*4; ++i) {
        EXPECT_EQ(state1.s[i], state2.s[i]);
    }
}

TEST(fill_canonical128_ps, Reentrant) {
    xorshift128_t state1;
    seed_xorshift128_urandom(&state1);

    xorshift128_t state2 = state1;

    std::vector<float> buffer(32);

    fill_canonical128_ps(buffer.data() +  0,  8, &state1);
    fill_canonical128_ps(buffer.data() +  8,  8, &state1);
    fill_canonical128_ps(buffer.data() + 16, 16, &state2);

    for(int i = 0; i < 16; ++i) {
        EXPECT_EQ(buffer[i], buffer[i+16]);
    }
    for(int i = 0; i < 2*4; ++i) {
        EXPECT_EQ(state1.s[i], state2.s[i]);
    }
}




TEST(fill_canonical128_pd, BufferSizeControl) {
    xorshift128_t state;
    for(int i = 0; i < 4; ++i) {
        state.s[i+0] = i+1;
        state.s[i+4] = 0;
    }
    double buffer[8] = {0};

    fill_canonical128_pd(buffer, 5, &state);

    for(int i = 0; i < 5; ++i) {
        EXPECT_NE(buffer[i], 0.0);
    }
    for(int i = 5; i < 8; ++i) {
        EXPECT_EQ(buffer[i], 0.0);
    }
}

TEST(fill_canonical128_pd, Distribution) {
    constexpr size_t N = 1ull<<21;
    xorshift128_t state;
    seed_xorshift128_urandom(&state);

    std::vector<double> buffer(N);

    fill_canonical128_pd(buffer.data(), N, &state);

    for(size_t i = 0; i < N; ++i) {
        EXPECT_GE(buffer[i], 0.0);
        EXPECT_LT(buffer[i], 1.0);
    }

    double mean = 0.0;
    for(size_t i = 0; i < N; ++i) {
        mean += buffer[i];
    }
    mean /= N;
    EXPECT_NEAR(mean, 0.5, 0.001);
}

TEST(fill_canonical128_pd, Repeatable) {
    xorshift128_t state1;
    seed_xorshift128_urandom(&state1);

    xorshift128_t state2 = state1;

    std::vector<double> buffer(16);

    fill_canonical128_pd(buffer.data() + 0, 8, &state1);
    fill_canonical128_pd(buffer.data() + 8, 8, &state2);

    for(int i = 0; i < 8; ++i) {
        EXPECT_EQ(buffer[i], buffer[i+8]);
    }
    for(int i = 0; i < 2*4; ++i) {
        EXPECT_EQ(state1.s[i], state2.s[i]);
    }
}

TEST(fill_canonical128_pd, Reentrant) {
    xorshift128_t state1;
    seed_xorshift128_urandom(&state1);

    xorshift128_t state2 = state1;

    std::vector<double> buffer(16);

    fill_canonical128_pd(buffer.data() + 0, 4, &state1);
    fill_canonical128_pd(buffer.data() + 4, 4, &state1);
    fill_canonical128_pd(buffer.data() + 8, 8, &state2);

    for(int i = 0; i < 8; ++i) {
        EXPECT_EQ(buffer[i], buffer[i+8]);
    }
    for(int i = 0; i < 2*4; ++i) {
        EXPECT_EQ(state1.s[i], state2.s[i]);
    }
}




TEST(fill_canonical1024_ps, Distribution) {
    constexpr size_t N = 1ull<<22;
    xorshift1024_t state;
    seed_xorshift1024_urandom(&state);

    std::vector<float> buffer(N);

    fill_canonical1024_ps(buffer.data(), N, &state);

    for(size_t i = 0; i < N; ++i) {
        EXPECT_GE(buffer[i], 0.0f);
        EXPECT_LT(buffer[i], 1.0f);
    }

    double mean = 0.0;
    for(size_t i = 0; i < N; ++i) {
        mean += buffer[i];
    }
    mean /= N;
    EXPECT_NEAR(mean, 0.5, 0.001);
}

TEST(fill_canonical1024_ps, Repeatable) {
    xorshift1024_t state1;
    seed_xorshift1024_urandom(&state1);

    xorshift1024_t state2 = state1;

    std::vector<float> buffer(32);

    fill_canonical1024_ps(buffer.data()+ 0, 16, &state1);
    fill_canonical1024_ps(buffer.data()+16, 16, &state2);

    for(size_t i = 0; i < 16; ++i) {
        EXPECT_EQ(buffer[i], buffer[i+16]);
        EXPECT_EQ(state1.s[4*i+0], state2.s[4*i+0]);
        EXPECT_EQ(state1.s[4*i+1], state2.s[4*i+1]);
        EXPECT_EQ(state1.s[4*i+2], state2.s[4*i+2]);
        EXPECT_EQ(state1.s[4*i+3], state2.s[4*i+3]);
    }
    EXPECT_EQ(state1.p, state2.p);
}

TEST(fill_canonical1024_ps, Reentrant) {
    xorshift1024_t state1;
    seed_xorshift1024_urandom(&state1);

    xorshift1024_t state2 = state1;

    std::vector<float> buffer(32);

    fill_canonical1024_ps(buffer.data()+ 0,  8, &state1);
    fill_canonical1024_ps(buffer.data()+ 8,  8, &state1);
    fill_canonical1024_ps(buffer.data()+16, 16, &state2);

    for(size_t i = 0; i < 16; ++i) {
        EXPECT_EQ(buffer[i], buffer[i+16]);
        EXPECT_EQ(state1.s[i], state2.s[i]);
    }
}




TEST(fill_canonical1024_pd, Distribution) {
    constexpr size_t N = 1ull<<21;
    xorshift1024_t state;
    seed_xorshift1024_urandom(&state);

    std::vector<double> buffer(N);

    fill_canonical1024_pd(buffer.data(), N, &state);

    for(size_t i = 0; i < N; ++i) {
        EXPECT_GE(buffer[i], 0.0);
        EXPECT_LT(buffer[i], 1.0);
    }

    double mean = 0.0;
    for(size_t i = 0; i < N; ++i) {
        mean += buffer[i];
    }
    mean /= N;
    EXPECT_NEAR(mean, 0.5, 0.001);
}

TEST(fill_canonical1024_pd, Repeatable) {
    xorshift1024_t state1;
    seed_xorshift1024_urandom(&state1);

    xorshift1024_t state2 = state1;

    std::vector<double> buffer(16);

    fill_canonical1024_pd(buffer.data()+0, 8, &state1);
    fill_canonical1024_pd(buffer.data()+8, 8, &state2);

    for(size_t i = 0; i < 8; ++i) {
        EXPECT_EQ(buffer[i], buffer[i+8]);
        EXPECT_EQ(state1.s[8*i+0], state2.s[8*i+0]);
        EXPECT_EQ(state1.s[8*i+1], state2.s[8*i+1]);
        EXPECT_EQ(state1.s[8*i+2], state2.s[8*i+2]);
        EXPECT_EQ(state1.s[8*i+3], state2.s[8*i+3]);
        EXPECT_EQ(state1.s[8*i+4], state2.s[8*i+4]);
        EXPECT_EQ(state1.s[8*i+5], state2.s[8*i+5]);
        EXPECT_EQ(state1.s[8*i+6], state2.s[8*i+6]);
        EXPECT_EQ(state1.s[8*i+7], state2.s[8*i+7]);
    }
    EXPECT_EQ(state1.p, state2.p);
}

TEST(fill_canonical1024_pd, Reentrant) {
    xorshift1024_t state1;
    seed_xorshift1024_urandom(&state1);

    xorshift1024_t state2 = state1;

    std::vector<double> buffer(16);

    fill_canonical1024_pd(buffer.data()+0, 4, &state1);
    fill_canonical1024_pd(buffer.data()+4, 4, &state1);
    fill_canonical1024_pd(buffer.data()+8, 8, &state2);

    for(size_t i = 0; i < 8; ++i) {
        EXPECT_EQ(buffer[i], buffer[i+8]);
    }
}
