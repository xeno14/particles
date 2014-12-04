#include <gtest/gtest.h>

#include "expression.hpp"

#include <tuple>

TEST(ExpressionTest, assign) {
  std::tuple<int, int> t;
  std::tuple<int, int> u(1, 2);

  particles::expression::assign<2>(t, u);
  EXPECT_EQ(1, std::get<0>(t));
  EXPECT_EQ(2, std::get<1>(t));
}
