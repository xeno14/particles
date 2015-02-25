#include <gtest/gtest.h>

#include "particles/expression.hpp"

#include <tuple>

using namespace particles;
using namespace particles::expression;

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

TEST(ExpressionTest, Assign) {
  std::array<int, 3> a;
  expression::assign(a, 1, 2, 3);

  EXPECT_EQ(1, a[0]);
  EXPECT_EQ(2, a[1]);
  EXPECT_EQ(3, a[2]);
}

TEST(ExpressionTest, inner_prod) {
  auto t = std::make_tuple(1, 2, 3);
  auto u = std::make_tuple(2, 3, 4);
  EXPECT_EQ(20, inner_prod(t, u));
}

TEST(ExpressionTest, inner_prod2) {
  EXPECT_EQ(8, inner_prod<2>(
    std::make_tuple(1, 2, 3, 4), std::make_tuple(2, 3)));
}

TEST(ExpressionTest, euclidean_norm) {
  EXPECT_DOUBLE_EQ(5, euclidean_norm(std::make_tuple(3, 4)));
}

class TupleForEachTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    t = std::make_tuple(1, 2, 3);
  }
  std::tuple<int, int, int> t;
};

TEST_F(TupleForEachTest, increment) {
  tuple_for_each(t, [](int& n) { n++; });
  EXPECT_EQ(2, std::get<0>(t));
  EXPECT_EQ(3, std::get<1>(t));
  EXPECT_EQ(4, std::get<2>(t));
}

TEST_F(TupleForEachTest, sum) {
  int sum = 0;
  tuple_for_each(t, [&](int n) { sum += n; });
  EXPECT_EQ(6, sum);
}

TEST_F(TupleForEachTest, use_auto) {
  std::tuple<int, double, char> u(0, 0, 0);
  tuple_for_each(u, [](auto& x) { x += 1; });
  EXPECT_EQ(1, std::get<0>(u));
  EXPECT_DOUBLE_EQ(1.0, std::get<1>(u));
  EXPECT_EQ(1, std::get<2>(u));
}

TEST(ExpressionTest, element_at) {
  int x,y,z;
  element_at<0>(x, y, z) = 100;
  EXPECT_EQ(100, x);
}

class ReturnTypeTest : public ::testing::Test {
  protected:
    static int  f(int n)  { return n; }
    static int& g(int& n) { return std::ref(n); }
    static auto h(std::tuple<int, int>& t) { return std::ref(t); }
};

TEST_F(ReturnTypeTest, value) {
  auto a = [](int n) { return n; };
  static_assert(
    std::is_same<int, return_type<decltype(a), int>>{},
    "mismatch");
}

TEST_F(ReturnTypeTest, inference_value) {
  static_assert(
    std::is_same<int, decltype(return_type_inferenece(f, 0))>{},
    "mismatch");
}

TEST_F(ReturnTypeTest, inference_ref) {
  int  n = 0;
  int& m = n;
  static_assert(
    std::is_same<int&, decltype(return_type_inferenece(g, m))>{},
    "mismatch");
}

TEST_F(ReturnTypeTest, inference_ref2) {
  auto t = std::make_tuple(1, 2);
  auto l = [](auto& x)->decltype(t)& { return std::ref(x); };
  std::get<0>(l(t)) = 0;
  EXPECT_EQ(0, std::get<0>(t));
  typedef decltype(return_type_inferenece(l, t)) type;
  static_assert(std::is_same<decltype(t)&, type>{}, "mismatch");
}
