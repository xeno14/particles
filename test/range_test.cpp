#include "particles/range.hpp"
#include "particles/vec.hpp"

#include <gtest/gtest.h>

#include <typeinfo>
#include <iostream>
#include <functional>
#include <vector>

using namespace particles;
using namespace particles::range;

template <class T, class U>
inline T& pair_first(std::pair<T, U>& p) { return p.first; }

template<class Itr>
inline auto first_iterator(Itr it) {
  return convert_iterator(
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
  EXPECT_EQ(6, sum(u.begin(), u.end()));

  std::vector<Vec<double, 2>> v = {{1, 2},{3, 4}};
  auto s = sum(v.begin(), v.end());
  EXPECT_DOUBLE_EQ(4, s[0]);
  EXPECT_DOUBLE_EQ(6, s[1]);

  std::vector<std::pair<int, int>> w = {{1,2},{3,4}};
  // auto cvt = [](std::pair<int, int>& p) { return p.first; };
  auto first = convert_iterator(w.begin(), pair_first<int, int>);
  auto last = convert_iterator(w.end(), pair_first<int,int>);
  auto t = sum(first, last);
  EXPECT_EQ(4, t);
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

class ZipTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    u = {1, 2, 3};
    v = {4, 5, 6};
  }

  std::vector<int> u;
  std::vector<int> v;
};

TEST_F(ZipTest, ref_tuple) {
  auto it = std::make_tuple(u.begin(), v.begin());
  auto t = range::internal::ref_tuple<range::internal::ref>(it);
  std::get<0>(t) -= 2;
  std::get<1>(t) += 6;
  EXPECT_EQ(-1, u[0]);
  EXPECT_EQ(10, v[0]);
}

TEST_F(ZipTest, ZipRange) {
  auto zipcon = ZipRange<decltype(u), decltype(v)>(u, v);

  // check overloads
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

TEST_F(ZipTest, zip) {
  for(auto z : zip(u, v)) {
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

TEST(RangeTest, XRange) {
  XRange<int> xrange(1, 4); // 1, 2, 3
  auto it = xrange.begin();

  EXPECT_EQ(1, *it); ++it;
  EXPECT_EQ(2, *(it++));
  EXPECT_EQ(3, *it); ++it;
  EXPECT_EQ(xrange.end(), it);
}

class xrangeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    result.clear();
  }
  std::vector<int> result;
};

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

TEST(RangeTest, push_back_iterator) {
  std::vector<int> v {1, 2, 3, 4};
  std::vector<int> res {0};
  std::copy(v.begin(), v.end(), push_back_iterator(res));
  ASSERT_EQ(5, res.size());
  EXPECT_EQ(0, res[0]);
  EXPECT_EQ(1, res[1]);
  EXPECT_EQ(2, res[2]);
  EXPECT_EQ(3, res[3]);
  EXPECT_EQ(4, res[4]);
}
