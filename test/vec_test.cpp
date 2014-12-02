#include "vec.hpp"

#include <gtest/gtest.h>

using particles::Vec;

TEST(VecTest, operators) {
  particles::Vec<double, 3> v1{0, 1, 2};
  particles::Vec<double, 3> v2{1, 2, 3};
  particles::Vec<double, 3> v3{2, 3, 4};
  particles::Vec<double, 3> result;

  auto zero = particles::Vec<double, 3>::zero();

  EXPECT_DOUBLE_EQ(0, zero[0]);
  EXPECT_DOUBLE_EQ(0, zero[1]);
  EXPECT_DOUBLE_EQ(0, zero[2]);

  result = v1;
  EXPECT_DOUBLE_EQ(0, result[0]);
  EXPECT_DOUBLE_EQ(1, result[1]);
  EXPECT_DOUBLE_EQ(2, result[2]);

  result = v1 + v2 + v3;
  EXPECT_DOUBLE_EQ(3, result[0]);
  EXPECT_DOUBLE_EQ(6, result[1]);
  EXPECT_DOUBLE_EQ(9, result[2]);
  EXPECT_DOUBLE_EQ(3, std::get<0>(result));
  EXPECT_DOUBLE_EQ(6, std::get<1>(result));
  EXPECT_DOUBLE_EQ(9, std::get<2>(result));

  result = v1 + v2 - v3;
  EXPECT_DOUBLE_EQ(-1, result[0]);
  EXPECT_DOUBLE_EQ(0, result[1]);
  EXPECT_DOUBLE_EQ(1, result[2]);

  result = v1 + v2;  // 1, 3, 5
  result *= 2;
  EXPECT_DOUBLE_EQ(2, result[0]);
  EXPECT_DOUBLE_EQ(6, result[1]);
  EXPECT_DOUBLE_EQ(10, result[2]);

  result = v1 * 2.0 + v2;
  EXPECT_DOUBLE_EQ(1, result[0]);
  EXPECT_DOUBLE_EQ(4, result[1]);
  EXPECT_DOUBLE_EQ(7, result[2]);

  result = v1 / 2 + v2;
  EXPECT_DOUBLE_EQ(1, result[0]);
  EXPECT_DOUBLE_EQ(2.5, result[1]);
  EXPECT_DOUBLE_EQ(4, result[2]);
}

TEST(VecTest, mathmatics) {
  Vec<double, 2> v1 {3, 4};
  Vec<double, 2> v2 {3, 6};

  EXPECT_DOUBLE_EQ(25, v1.squared_length());
  EXPECT_DOUBLE_EQ(5, v1.length());
  EXPECT_DOUBLE_EQ(4, v1.squared_distance(v2));
  EXPECT_DOUBLE_EQ(2, v1.distance(v2));
}
