#include "range.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <vector>

using namespace particles;

template<class Itr>
auto first_iterator(Itr it) {
  static auto cvt = [](std::vector<int>& u) { return u[0]; };
  return range::convert_iterator(it, cvt);
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

TEST(RangeTest, ref_tuple) {
  std::vector<int> v {1, 2, 3}, u {4, 5, 6};
  auto it = std::make_tuple(v.begin(), u.begin());
  auto t = range::internal::ref_tuple(it);
}
