#include <gtest/gtest.h>

#include "particles/range/enumerate.hpp"

using namespace particles;
using namespace particles::range;


class EnumerateTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    v = {-1, -3, -5};
    indices.clear();
  }
  std::vector<int> v;
  std::vector<int> indices;
};

TEST_F(EnumerateTest, EnumerateRange) {
  EnumerateRange<decltype(v)> enum_range(v, 0);
  auto first = enum_range.begin();
  auto last = enum_range.end();

  auto it = first;
  EXPECT_EQ(0, std::get<0>(*it));
  EXPECT_EQ(-1, std::get<1>(*it)); ++it;
  EXPECT_EQ(1, std::get<0>(*it));
  EXPECT_EQ(-3, std::get<1>(*(it++)));

  // Reference check
  std::get<1>(*it) *= 2;
  EXPECT_EQ(2, std::get<0>(*it));
  EXPECT_EQ(-10, v[2]);

  ++it;
  EXPECT_EQ(last, it);

  EXPECT_EQ(-1, v[0]);
  EXPECT_EQ(-3, v[1]);
  EXPECT_EQ(-10, v[2]);
}

TEST_F(EnumerateTest, EnumerateRange2) {
  EnumerateRange<decltype(v)> enum_range(v, 1);
  auto it = enum_range.begin();

  // Doubles each element
  while (it != enum_range.end()) {
    indices.push_back(std::get<0>(*it));
    std::get<1>(*it) *= 2;
    ++it;
  }
  EXPECT_EQ(1, indices[0]);
  EXPECT_EQ(2, indices[1]);
  EXPECT_EQ(3, indices[2]);
  EXPECT_EQ(-2, v[0]);
  EXPECT_EQ(-6, v[1]);
  EXPECT_EQ(-10, v[2]);
}

TEST_F(EnumerateTest, enumerate) {
  for (auto t : enumerate(v, -2)) {
    indices.push_back(std::get<0>(t));
    std::get<1>(t) *= -1; // reference check
    t.second += 1;
  }
  EXPECT_EQ(-2, indices[0]);
  EXPECT_EQ(-1, indices[1]);
  EXPECT_EQ( 0, indices[2]);
  EXPECT_EQ(2, v[0]);
  EXPECT_EQ(4, v[1]);
  EXPECT_EQ(6, v[2]);
}
