#include <gtest/gtest.h>

#include "particles/range/zip.hpp"

using namespace particles;
using namespace particles::range;


class xrangeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    result.clear();
  }
  std::vector<int> result;
};

TEST_F(xrangeTest, iterator) {
  XRange<int> rng(1, 4); // 1, 2, 3
  auto it = rng.begin();

  EXPECT_EQ(1, *it); ++it;
  EXPECT_EQ(2, *it++);
  EXPECT_EQ(3, *it); ++it;
  EXPECT_EQ(rng.end(), it);
}

TEST_F(xrangeTest, last_only) {
  for(auto n : xrange(3)) {
    result.push_back(n);
  }
  EXPECT_EQ(0, result[0]);
  EXPECT_EQ(1, result[1]);
  EXPECT_EQ(2, result[2]);
}

TEST_F(xrangeTest, first_and_last) {
  for(auto n : xrange(-3, 0)) {
    result.push_back(n);
  }
  EXPECT_EQ(-3, result[0]);
  EXPECT_EQ(-2, result[1]);
  EXPECT_EQ(-1, result[2]);
}

TEST_F(xrangeTest, step) {
  for (auto n : xrange(0, 6, 2)) {
    result.push_back(n);
  }
  EXPECT_EQ(0, result[0]);
  EXPECT_EQ(2, result[1]);
  EXPECT_EQ(4, result[2]);
}

TEST_F(xrangeTest, step2) {
  for (auto n : xrange(0, 5, 2)) {
    result.push_back(n);
  }
  EXPECT_EQ(0, result[0]);
  EXPECT_EQ(2, result[1]);
  EXPECT_EQ(4, result[2]);
}

TEST_F(xrangeTest, construct_stl_container) {
  auto rng = xrange(3);
  std::vector<int> v(rng.begin(), rng.end());
  EXPECT_EQ(0, v[0]);
  EXPECT_EQ(1, v[1]);
  EXPECT_EQ(2, v[2]);
}

// Seg fault...
// TEST_F(xrangeTest, stl_transform) {
//   auto rng = xrange(3);
//   std::transform(std::begin(rng), std::end(rng), result.begin(),
//                  [](int n) { return n*2; });
// }

