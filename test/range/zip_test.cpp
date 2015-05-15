#include <gtest/gtest.h>

#include "particles/range/zip.hpp"

using namespace particles;
using namespace particles::range;


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
