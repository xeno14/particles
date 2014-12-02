#include "vec.hpp"

#include <gtest/gtest.h>

TEST(VecTest, construct) {
  particles::Vec3<double> v0;
  EXPECT_DOUBLE_EQ(0, v0.x);
  EXPECT_DOUBLE_EQ(0, v0.y);
  EXPECT_DOUBLE_EQ(0, v0.z);
}
