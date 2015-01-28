#include <gtest/gtest.h>

#include "particles/ET.hpp"

#include <tuple>
#include <vector>


using namespace particles;
using namespace particles::ET;

TEST(PairTest, pair0) {
  constexpr auto i = Pair<0>::first;
  constexpr auto j = Pair<0>::second;
  EXPECT_EQ(1, i);
  EXPECT_EQ(2, j);
}

TEST(PairTest, pair1) {
  constexpr auto i = Pair<1>::first;
  constexpr auto j = Pair<1>::second;
  EXPECT_EQ(2, i);
  EXPECT_EQ(0, j);
}

TEST(PairTest, pair2) {
  constexpr auto i = Pair<2>::first;
  constexpr auto j = Pair<2>::second;
  EXPECT_EQ(0, i);
  EXPECT_EQ(1, j);
}

TEST(SignTest, sign00) {
  constexpr auto s = Sign<0, 0>::value;
  EXPECT_EQ(0, s);
}

TEST(SignTest, sign01) {
  constexpr auto s = Sign<0, 1>::value;
  EXPECT_EQ(1, s);
}

TEST(SignTest, sign02) {
  constexpr auto s = Sign<0, 2>::value;
  EXPECT_EQ(-1, s);
}

TEST(SignTest, sign10) {
  constexpr auto s = Sign<1, 0>::value;
  EXPECT_EQ(-1, s);
}

TEST(SignTest, sign11) {
  constexpr auto s = Sign<1, 1>::value;
  EXPECT_EQ(0, s);
}

TEST(SignTest, sign12) {
  constexpr auto s = Sign<1, 2>::value;
  EXPECT_EQ(1, s);
}

TEST(SignTest, sign20) {
  constexpr auto s = Sign<2, 0>::value;
  EXPECT_EQ(1, s);
}

TEST(SignTest, sign21) {
  constexpr auto s = Sign<2, 1>::value;
  EXPECT_EQ(-1, s);
}

TEST(SignTest, sign22) {
  constexpr auto s = Sign<2, 2>::value;
  EXPECT_EQ(0, s);
}

class CrossTest : public ::testing::Test {
 protected:
  void SetUp() {
    e0 = std::make_tuple(1, 0, 0);
    e1 = std::make_tuple(0, 1, 0);
    e2 = std::make_tuple(0, 0, 1);
  }
  std::tuple<int, int, int> e0, e1, e2;
};

TEST_F(CrossTest, cross_00) {
  auto result = cross(e0, e0);
  EXPECT_EQ(0, result.get<0>());
  EXPECT_EQ(0, result.get<1>());
  EXPECT_EQ(0, result.get<2>());
}

TEST_F(CrossTest, cross_01) {
  auto result = cross(e0, e1);
  EXPECT_EQ(0, result.get<0>());
  EXPECT_EQ(0, result.get<1>());
  EXPECT_EQ(1, result.get<2>());
}

TEST_F(CrossTest, cross_02) {
  auto result = cross(e0, e2);
  EXPECT_EQ( 0, result.get<0>());
  EXPECT_EQ(-1, result.get<1>());
  EXPECT_EQ( 0, result.get<2>());
}

TEST_F(CrossTest, cross_10) {
  auto result = cross(e1, e0);
  EXPECT_EQ( 0, result.get<0>());
  EXPECT_EQ( 0, result.get<1>());
  EXPECT_EQ(-1, result.get<2>());
}

TEST_F(CrossTest, cross_11) {
  auto result = cross(e1, e1);
  EXPECT_EQ(0, result.get<0>());
  EXPECT_EQ(0, result.get<1>());
  EXPECT_EQ(0, result.get<2>());
}

TEST_F(CrossTest, cross_12) {
  auto result = cross(e1, e2);
  EXPECT_EQ(1, result.get<0>());
  EXPECT_EQ(0, result.get<1>());
  EXPECT_EQ(0, result.get<2>());
}

TEST_F(CrossTest, cross_20) {
  auto result = cross(e2, e0);
  EXPECT_EQ(0, result.get<0>());
  EXPECT_EQ(1, result.get<1>());
  EXPECT_EQ(0, result.get<2>());
}

TEST_F(CrossTest, cross_21) {
  auto result = cross(e2, e1);
  EXPECT_EQ(-1, result.get<0>());
  EXPECT_EQ( 0, result.get<1>());
  EXPECT_EQ( 0, result.get<2>());
}

TEST_F(CrossTest, cross_22) {
  auto result = cross(e2, e2);
  EXPECT_EQ(0, result.get<0>());
  EXPECT_EQ(0, result.get<1>());
  EXPECT_EQ(0, result.get<2>());
}

TEST_F(CrossTest, std_get) {
  auto result = cross(e0, e1);
  EXPECT_EQ(0, std::get<0>(result));
  EXPECT_EQ(0, std::get<1>(result));
  EXPECT_EQ(1, std::get<2>(result));
}

TEST(CrossOperatorTest, use_operator) {
  std::vector<int> e0 {1, 0, 0};
  std::vector<int> e1 {0, 1, 0};
  auto result = cross(e0, e1);
  EXPECT_EQ(0, result[0]);
  EXPECT_EQ(0, result[1]);
  EXPECT_EQ(1, result[2]);
}
