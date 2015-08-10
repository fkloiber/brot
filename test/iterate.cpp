#include <cstdio>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>
#include <brot/iterate.h>

TEST(test_in_M_ps, test_in_M_ps)
{
    std::vector<float> c(16);
    std::vector<uint32_t> i(8);

    c[ 0] =  0.0f;
    c[ 8] =  0.0f;
    c[ 1] =  2.0f;
    c[ 9] =  2.5f;
    c[ 2] =  1.0f;
    c[10] =  0.0f;
    c[ 3] = -1.0f;
    c[11] =  0.0f;
    c[ 4] =  0.0f;
    c[12] =  1.0f;
    c[ 5] =  0.0f;
    c[13] = -1.0f;
    c[ 6] = -0.75f;
    c[14] =  0.1f;
    c[ 7] =  0.275f;
    c[15] =  0.0f;

    test_in_M_ps(c.data(), i.data(), 8, 100, 2.0f);
    EXPECT_EQ(i[0], 100u);
    EXPECT_EQ(i[1],   0u);
    EXPECT_EQ(i[2],   2u);
    EXPECT_EQ(i[3], 100u);
    EXPECT_EQ(i[4], 100u);
    EXPECT_EQ(i[5], 100u);
    EXPECT_EQ(i[6],  32u);
    EXPECT_EQ(i[7],  17u);
}
