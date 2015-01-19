#include "particles/range.hpp"
#include "particles/vec.hpp"

#include <gtest/gtest.h>

#include <typeinfo>
#include <iostream>
#include <functional>
#include <vector>

using namespace particles;

template <class T, class U>
inline T& pair_first(std::pair<T, U>& p) { return p.first; }

template<class Itr>
inline auto first_iterator(Itr it) {
  return range::convert_iterator(
      it, [](std::pair<int, int>& u) { return std::ref(u.first); });
}

TEST(RangeTest, ConvertIterator) {
  std::vector<std::pair<int, int>> v{{1, 2}, {3, 4}, {5, 6}, {7, 8}};

  auto first = first_iterator(v.begin());
  auto last = first_iterator(v.end());
  auto it = first;
  EXPECT_EQ(1, *it); ++it;
  EXPECT_EQ(3, *(it++));
  EXPECT_EQ(5, *(it++));
  EXPECT_EQ(7, *(it++));
  EXPECT_TRUE(it == last);
  EXPECT_TRUE(it == v.end());

  // reference check
  it = first;
  (*it) *= -1;
  EXPECT_EQ(-1, v[0].first);
  EXPECT_EQ( 2, v[0].second);
}

TEST(RangeTest, sum) {
  std::vector<int> u = {1, 2, 3};
  EXPECT_EQ(6, range::sum(u.begin(), u.end()));

  std::vector<Vec<double, 2>> v = {{1, 2},{3, 4}};
  auto s = range::sum(v.begin(), v.end());
  EXPECT_DOUBLE_EQ(4, s[0]);
  EXPECT_DOUBLE_EQ(6, s[1]);

  std::vector<std::pair<int, int>> w = {{1,2},{3,4}};
  // auto cvt = [](std::pair<int, int>& p) { return p.first; };
  auto first = range::convert_iterator(w.begin(), pair_first<int, int>);
  auto last = range::convert_iterator(w.end(), pair_first<int,int>);
  auto t = range::sum(first, last);
  EXPECT_EQ(4, t);
}

TEST(RangeTest, average) {
  std::vector<Vec<double, 2>> v = {{1, 2},{3, 4}};
  auto ave = range::average(v.begin(), v.end());
  EXPECT_EQ(2, ave[0]);
  EXPECT_EQ(3, ave[1]);
}

/** @todo use TEST_F */
TEST(RangeTest, ref_tuple) {
  std::vector<int> u {1, 2, 3}, v {4, 5, 6};
  auto it = std::make_tuple(u.begin(), v.begin());
  auto t = range::internal::ref_tuple<range::internal::ref>(it);
  std::get<0>(t) -= 2;
  std::get<1>(t) += 6;
  EXPECT_EQ(-1, u[0]);
  EXPECT_EQ(10, v[0]);
}

TEST(RangeTest, ZipRange) {
  std::vector<int> u {1, 2, 3}, v {4, 5, 6};

  auto zipcon = range::ZipRange<decltype(u), decltype(v)>(u, v);

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

TEST(RangeTest, XRange) {
  range::XRange<int> xrange(1, 4); // 1, 2, 3
  auto it = xrange.begin();

  EXPECT_EQ(1, *it); ++it;
  EXPECT_EQ(2, *(it++));
  EXPECT_EQ(3, *it); ++it;
  EXPECT_EQ(xrange.end(), it);
}

TEST(RangeTest, xrange) {
  std::vector<int> result;

  result.clear();
  for(auto n : range::xrange(-3, 0)) {
    result.push_back(n);
  }
  EXPECT_EQ(-3, result[0]);
  EXPECT_EQ(-2, result[1]);
  EXPECT_EQ(-1, result[2]);

  result.clear();
  for(auto n : range::xrange(3)) {
    result.push_back(n);
    n++;  // reference does not harm iterator
  }
  EXPECT_EQ(0, result[0]);
  EXPECT_EQ(1, result[1]);
  EXPECT_EQ(2, result[2]);
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

// TEST(RangeTest, zip_const) {
//   std::vector<int> u {1, 2, 3}, v {4, 5, 6};
//   const auto& cu = u;
//   const auto& cv = v;
//   for(auto z : range::zip(cu, cv)) {
//   }
// }

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
  std::vector<int> indexes;

  range::EnumerateRange<decltype(v)> enum_range(v, 1);
  auto it = enum_range.begin();

  // Doubles each element
  while (it != enum_range.end()) {
    indexes.push_back(std::get<0>(*it));
    std::get<1>(*it) *= 2;
    ++it;
  }
  EXPECT_EQ(1, indexes[0]);
  EXPECT_EQ(2, indexes[1]);
  EXPECT_EQ(3, indexes[2]);
  EXPECT_EQ(-2, v[0]);
  EXPECT_EQ(-4, v[1]);
  EXPECT_EQ(-6, v[2]);
}

TEST(RangeTest, enumerate) {
  std::vector<int> v {-1, -2, -3};
  std::vector<int> indexes;

  for (auto t : range::enumerate(v, -2)) {
    indexes.push_back(std::get<0>(t));
    std::get<1>(t) *= -1; // reference check
    t.second += 1;
  }
  EXPECT_EQ(-2, indexes[0]);
  EXPECT_EQ(-1, indexes[1]);
  EXPECT_EQ( 0, indexes[2]);
  EXPECT_EQ(2, v[0]);
  EXPECT_EQ(3, v[1]);
  EXPECT_EQ(4, v[2]);
}
