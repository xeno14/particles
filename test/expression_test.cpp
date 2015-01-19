#include <gtest/gtest.h>

#include "particles/expression.hpp"

#include <tuple>

using namespace particles;

TEST(ExpressionTest, assign) {
  std::tuple<int, int> t;
  std::tuple<int, int> u(1, 2);

  expression::assign<2>(t, u);
  EXPECT_EQ(1, std::get<0>(t));
  EXPECT_EQ(2, std::get<1>(t));

  std::tuple<int, int, int, int> uu(0, 1, 2, 3);
  expression::assign_from_even<2>(t, uu);
  EXPECT_EQ(0, std::get<0>(t));
  EXPECT_EQ(2, std::get<1>(t));

  expression::assign_from_odd<2>(t, uu);
  EXPECT_EQ(1, std::get<0>(t));
  EXPECT_EQ(3, std::get<1>(t));
}

struct AlwaysZero {
  constexpr std::size_t operator()(std::size_t) { return 0; }
};

TEST(ExpressionTest, pre_increment) {
  auto t = std::make_tuple(1, 2);

  // increments once for each
  expression::pre_increment<2>(t);
  EXPECT_EQ(2, std::get<0>(t));
  EXPECT_EQ(3, std::get<1>(t));

  t = std::make_tuple(1, 2);
  expression::pre_increment<2>(t, AlwaysZero());
  EXPECT_EQ(3, std::get<0>(t));   // incremented twice
  EXPECT_EQ(2, std::get<1>(t));   // Not incremented
}
