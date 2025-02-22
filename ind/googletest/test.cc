#include <gtest/gtest.h>

#include <numeric>

TEST(DEQ, Normal) {
  const double zero = std::numeric_limits<float>::epsilon();
  const double eps = std::numeric_limits<double>::epsilon();

  EXPECT_EQ(zero, eps);
  EXPECT_EQ(eps, zero);
  EXPECT_FLOAT_EQ(zero, eps);
  EXPECT_FLOAT_EQ(eps, zero);
  EXPECT_DOUBLE_EQ(zero, eps);
  EXPECT_DOUBLE_EQ(eps, zero);
}
