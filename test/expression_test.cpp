#include <gtest/gtest.h>

#include "expression.hpp"

#include <tuple>

TEST(ExpressionTest, assign) {
  std::tuple<int, int> t;
  std::tuple<int, int> u(1, 2);

  particles::expression::assign<2>(t, u);
  EXPECT_EQ(1, std::get<0>(t));
  EXPECT_EQ(2, std::get<1>(t));

  std::tuple<int, int, int, int> uu(0, 1, 2, 3);
  particles::expression::assign_from_even<2>(t, uu);
  EXPECT_EQ(0, std::get<0>(t));
  EXPECT_EQ(2, std::get<1>(t));

  particles::expression::assign_from_odd<2>(t, uu);
  EXPECT_EQ(1, std::get<0>(t));
  EXPECT_EQ(3, std::get<1>(t));
}
