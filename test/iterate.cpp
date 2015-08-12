#include <cstdio>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>
#include <brot/iterate.h>

TEST(escape_test_ps, escape_test_ps)
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
