#include "particles/range.hpp"
#include "particles/vec.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <functional>
#include <vector>

using namespace particles;
using namespace particles::range;


TEST(RangeTest, sum) {
  std::vector<int> u = {1, 2, 3};
  EXPECT_EQ(6, sum(u.begin(), u.end()));

  std::vector<Vec<double, 2>> v = {{1, 2},{3, 4}};
  auto s = sum(v.begin(), v.end());
  EXPECT_DOUBLE_EQ(4, s[0]);
  EXPECT_DOUBLE_EQ(6, s[1]);
}

TEST(RangeTest, average) {
  std::vector<Vec<double, 2>> v = {{1, 2},{3, 4}};
  auto ave = average(v.begin(), v.end());
  EXPECT_DOUBLE_EQ(2, ave[0]);
  EXPECT_DOUBLE_EQ(3, ave[1]);
}

TEST(RangeTest, average2) {
  std::vector<int> v {1, 2};
  // double
  auto ave = average(v.begin(), v.end());
  EXPECT_DOUBLE_EQ(1.5, ave);
}


TEST(RangeTest, suite_last_test) {
  EXPECT_EQ( 3, suite_last(0,  3, 1));
  EXPECT_EQ( 6, suite_last(0,  5, 3));
  EXPECT_EQ( 6, suite_last(0,  6, 3));
  EXPECT_EQ( 9, suite_last(0,  7, 3));
  EXPECT_EQ( 9, suite_last(0,  8, 3));
  EXPECT_EQ( 9, suite_last(0,  9, 3));
  EXPECT_EQ(12, suite_last(0, 10, 3));
}
