#include "range.hpp"

#include <gtest/gtest.h>

#include <typeinfo>
#include <iostream>
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

/** @todo use TEST_F */
TEST(RangeTest, ref_tuple) {
  std::vector<int> u {1, 2, 3}, v {4, 5, 6};
  auto it = std::make_tuple(u.begin(), v.begin());
  auto t = range::internal::ref_tuple(it);
  std::get<0>(t) -= 2;
  std::get<1>(t) += 6;
  EXPECT_EQ(-1, u[0]);
  EXPECT_EQ(10, v[0]);
}

TEST(RangeTest, ZipContainer) {
  std::vector<int> u {1, 2, 3}, v {4, 5, 6};

  auto zipcon = range::ZipContainer<decltype(u), decltype(v)>(u, v);

  EXPECT_EQ(zipcon.begin(), std::begin(zipcon));
  EXPECT_EQ(zipcon.end(), std::end(zipcon));

  auto it = zipcon.begin();
  auto first = zipcon.begin();
  auto last = zipcon.end();

  EXPECT_EQ(first, it);

  EXPECT_EQ(1, std::get<0>(*it));
  std::get<0>(*it)--;
  EXPECT_EQ(0, std::get<0>(*it));
  EXPECT_EQ(4, std::get<1>(*it));
  ++it;
  
  EXPECT_EQ(2, std::get<0>(*it));
  EXPECT_EQ(5, std::get<1>(*(it++)));

  EXPECT_EQ(3, std::get<0>(*it));
  EXPECT_EQ(6, std::get<1>(*it));
  ++it;

  EXPECT_EQ(last, it);
}

TEST(RangeTest, zip) {
  std::vector<int> u {1, 2, 3}, v {4, 5, 6};

  for(auto z : range::zip(u, v)) {
    std::get<0>(z)--;
    std::get<1>(z)--;
  }
  EXPECT_EQ(0, u[0]);
  EXPECT_EQ(1, u[1]);
  EXPECT_EQ(2, u[2]);
  EXPECT_EQ(3, v[0]);
  EXPECT_EQ(4, v[1]);
  EXPECT_EQ(5, v[2]);
}

TEST(RangeTest, EnumerateRange) {
  std::vector<int> v {-1, -3, -5};

  range::EnumerateRange<decltype(v)> enum_range(v, 0);
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

TEST(RangeTest, EnumerateRange2) {
  std::vector<int> v {-1, -2, -3};

  range::EnumerateRange<decltype(v)> enum_range(v, 1);
  auto it = enum_range.begin();

  // v = [-2, -4, -6]
  int i=1;
  while (it != enum_range.end()) {
    std::get<1>(*it) *= 2;
    EXPECT_EQ(i, std::get<0>(*it));
    EXPECT_EQ(-i*2, v[i-1]);  // reference check
    ++it;
    i++;
  }
}

TEST(RangeTest, enumerate) {
  std::vector<int> v {-1, -2, -3};

  auto enum_range = range::enumerate(v, 0); /** @todo problem here? */
  auto it = enum_range.begin();
  int i=0;
  while (it != enum_range.end()) {
    std::get<1>(*it) *= -1;
    std::cerr << std::get<1>(*it) << " " << v[i] << std::endl;
    it++;
  }
}
