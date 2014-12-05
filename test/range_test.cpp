#include "range.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <vector>

using particles::range::ConvertIterator;

template<class Itr>
auto first_iterator(Itr it) {
  static auto cvt = [](std::vector<int>& u) { return u[0]; };
  return particles::range::convert_iterator(it, cvt);
}

TEST(RangeTest, ConvertIterator) {
  std::vector<std::vector<int>> v{{1, 2}, {3, 4}, {5, 6}, {7, 8}};

  auto first = first_iterator(v.begin());
  auto last = first_iterator(v.end());
  auto it = first;
  EXPECT_EQ(1, *it); ++it;
  EXPECT_EQ(3, *(it++));
  EXPECT_EQ(5, *(it++));
  EXPECT_EQ(7, *(it++));
  EXPECT_TRUE(it == last);
  EXPECT_TRUE(it == v.end());
}
