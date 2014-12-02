#include "vec.hpp"

#include <gtest/gtest.h>

TEST(VecTest, construct) {
  particles::Vec<double, 3> v1, v2, v3;
  v1[0] = 1; v2[0] = 2;
  v3 = v1 + v2;

  EXPECT_DOUBLE_EQ(3, v3[0]);
}
