#include "vec.hpp"

#include <gtest/gtest.h>

TEST(VecTest, operators) {
  particles::Vec<double, 3> v1 {0, 1, 2};
  particles::Vec<double, 3> v2 {1, 2, 3};
  particles::Vec<double, 3> v3 {2, 3, 4};
  particles::Vec<double, 3> result;

  result = v1 + v2 + v3;
  EXPECT_DOUBLE_EQ(3, result[0]);
  EXPECT_DOUBLE_EQ(6, result[1]);
  EXPECT_DOUBLE_EQ(9, result[2]);

  result = v1 + v2 - v3;
  EXPECT_DOUBLE_EQ(-1, result[0]);
  EXPECT_DOUBLE_EQ(0, result[1]);
  EXPECT_DOUBLE_EQ(1, result[2]);
}
