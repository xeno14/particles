#include "particles/vec.hpp"

#include <gtest/gtest.h>

using particles::Vec;

class OperatorTest : public ::testing::Test {
 protected:
  OperatorTest () {}
  ~OperatorTest() {}
  virtual void SetUp() {
    v1 = {0, 1, 2};
    v2 = {1, 2, 3};
    v3 = {2, 3, 4};
    result = decltype(result)::zero();
  }
  Vec<double, 3> v1{0, 1, 2};
  Vec<double, 3> v2{1, 2, 3};
  Vec<double, 3> v3{2, 3, 4};
  Vec<double, 3> result;
};

TEST_F(OperatorTest, zero) {
  auto zero = particles::Vec<double, 3>::zero();

  EXPECT_DOUBLE_EQ(0, zero[0]);
  EXPECT_DOUBLE_EQ(0, zero[1]);
  EXPECT_DOUBLE_EQ(0, zero[2]);
}

TEST_F(OperatorTest, fill) {
  result.fill(1);
  EXPECT_DOUBLE_EQ(1, result[0]);
  EXPECT_DOUBLE_EQ(1, result[1]);
  EXPECT_DOUBLE_EQ(1, result[2]);
}

TEST_F(OperatorTest, assign) {
  result = v1;
  EXPECT_DOUBLE_EQ(0, result[0]);
  EXPECT_DOUBLE_EQ(1, result[1]);
  EXPECT_DOUBLE_EQ(2, result[2]);
}

TEST_F(OperatorTest, add) {
  result = v1 + v2 + v3;
  EXPECT_DOUBLE_EQ(3, result[0]);
  EXPECT_DOUBLE_EQ(6, result[1]);
  EXPECT_DOUBLE_EQ(9, result[2]);
  EXPECT_DOUBLE_EQ(3, std::get<0>(result));
  EXPECT_DOUBLE_EQ(6, std::get<1>(result));
  EXPECT_DOUBLE_EQ(9, std::get<2>(result));
}

TEST_F(OperatorTest, plus_and_minus) {
  result = v1 + v2 - v3;
  EXPECT_DOUBLE_EQ(-1, result[0]);
  EXPECT_DOUBLE_EQ(0, result[1]);
  EXPECT_DOUBLE_EQ(1, result[2]);
}

TEST_F(OperatorTest, multiply) {
  result = v1 + v2;  // 1, 3, 5
  result *= 2;
  EXPECT_DOUBLE_EQ(2, result[0]);
  EXPECT_DOUBLE_EQ(6, result[1]);
  EXPECT_DOUBLE_EQ(10, result[2]);

  result = v2 * 2.0;
  EXPECT_DOUBLE_EQ(2, result[0]);
  EXPECT_DOUBLE_EQ(4, result[1]);
  EXPECT_DOUBLE_EQ(6, result[2]);
}

TEST_F(OperatorTest, multiply_and_plus) {
  result = v1 * 2.0 + v2;
  EXPECT_DOUBLE_EQ(1, result[0]);
  EXPECT_DOUBLE_EQ(4, result[1]);
  EXPECT_DOUBLE_EQ(7, result[2]);

  result = v1 + v2 * 2.0;
  EXPECT_DOUBLE_EQ(2, result[0]);
  EXPECT_DOUBLE_EQ(5, result[1]);
  EXPECT_DOUBLE_EQ(8, result[2]);
}

TEST_F(OperatorTest, devide) {
  result = v1 / 2 + v2;
  EXPECT_DOUBLE_EQ(1, result[0]);
  EXPECT_DOUBLE_EQ(2.5, result[1]);
  EXPECT_DOUBLE_EQ(4, result[2]);

  result = v1 + v2 / 2;
  EXPECT_DOUBLE_EQ(0.5, result[0]);
  EXPECT_DOUBLE_EQ(2, result[1]);
  EXPECT_DOUBLE_EQ(3.5, result[2]);
}

TEST_F(OperatorTest, plus_and_assign) {
  result += v1 + v2;
  EXPECT_DOUBLE_EQ(result[0], 1);
  EXPECT_DOUBLE_EQ(result[1], 3);
  EXPECT_DOUBLE_EQ(result[2], 5);
}

TEST_F(OperatorTest, compare) {
  const auto& z = decltype(result)::zero();
  result = z; 
  EXPECT_TRUE(result == z);
  EXPECT_FALSE(result == v1);
  EXPECT_TRUE(result != v1);
  EXPECT_FALSE(result != z);
}

TEST_F(OperatorTest, exp_and_scalar) {
  result = (v1 + v3) / 2 + v2;
  EXPECT_DOUBLE_EQ(2, result[0]);
  EXPECT_DOUBLE_EQ(4, result[1]);
  EXPECT_DOUBLE_EQ(6, result[2]);
}

TEST_F(OperatorTest, inner_product) {
  EXPECT_DOUBLE_EQ(8, v1 * v2);
}


TEST(MathTest, length) {
  Vec<double, 2> v1 {3, 4};
  Vec<double, 2> v2 {3, 6};

  EXPECT_DOUBLE_EQ(25, v1.squared_length());
  EXPECT_DOUBLE_EQ(5, v1.length());
  EXPECT_DOUBLE_EQ(4, v1.squared_distance(v2));
  EXPECT_DOUBLE_EQ(2, v1.distance(v2));
  EXPECT_DOUBLE_EQ(33, v1.dot(v2));
}

TEST(MathTest, dot) {
  Vec<int, 2> u(1, 2), v(3, 4);
  EXPECT_EQ(11, u.dot(v));
}

TEST(MathTest, parallel) {
  Vec<double, 2> u {3, 4};
  EXPECT_TRUE( u.parallel({3, 4}));
  EXPECT_TRUE( u.parallel({3, 4}, 1e-4));
  EXPECT_FALSE(u.parallel({3, 6}, 1e-6));
  EXPECT_FALSE(u.parallel({3, 6}, 1e-6));
  EXPECT_TRUE( u.parallel({3, 6}, 1));
}

TEST(MathTest, normalize) {
  Vec<double, 2> n;
  n = {10, 0};
  n.normalize(100);
  EXPECT_TRUE(n.parallel({1, 0}));
  EXPECT_DOUBLE_EQ(100, n.length());

  n = {3, 4};
  n.normalize();
  EXPECT_TRUE(n.parallel({3000, 4000}));
  EXPECT_DOUBLE_EQ(1, n.length());
}

class OverloadTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    u = {0, 1};
    v = {1, 2};
  }
  Vec<int, 2> u;
  Vec<int, 2> v;
};

TEST_F(OverloadTest, get) {
  EXPECT_EQ(1, std::get<0>(v));
  EXPECT_EQ(2, std::get<1>(v));

  std::get<0>(v) = 100;  // reference
  EXPECT_EQ(100, v[0]);

  auto f = [](const decltype(v)& u) {
    return std::get<0>(u);
  };

  EXPECT_EQ(100, f(v));
}

TEST_F(OverloadTest, begin) {
  EXPECT_EQ(1, *std::begin(v));
}

TEST_F(OverloadTest, tuple_size) {
  EXPECT_EQ(2, std::tuple_size<decltype(v)>::value);
}

TEST_F(OverloadTest, swap) {
  std::swap(u, v);
  EXPECT_EQ(1, u[0]);
  EXPECT_EQ(2, u[1]);
  EXPECT_EQ(0, v[0]);
  EXPECT_EQ(1, v[1]);
}

TEST(VecTest, construct_variable_args) {
  Vec<int, 3> v(1, 2, 3);
  EXPECT_EQ(1, v[0]);
  EXPECT_EQ(2, v[1]);
  EXPECT_EQ(3, v[2]);
}

TEST(VecTest, construct_expression) {
  Vec<int, 2> a(1, 2), b(3, 4);
  Vec<int, 2> v = a + b;  // Exp<Vec, Plus, Vec>
  EXPECT_EQ(4, v[0]);
  EXPECT_EQ(6, v[1]);
}

TEST(VecTest, inner_prod) {
  Vec<int, 3> a(1, 2, 3), b(2, 3, 4);
  EXPECT_EQ(20, inner_prod<3>(a, b));
}

TEST(VecTest, euclidean_norm) {
  Vec<int, 2> a(4, 5), b(1, 1);
  EXPECT_DOUBLE_EQ(5, euclidean_norm<2>(a - b));
}

class CrossTest : public ::testing::Test {
 protected:
  virtual void SetUp() { result.fill(0); }
  Vec<int, 3> result;
};

TEST_F(CrossTest, parallel) {
  Vec<int, 3> a{1, 2, 3}, b{2, 4, 6};
  result = cross(a, b);
  EXPECT_EQ(0, result.squared_length());
}

TEST_F(CrossTest, ET) {
  Vec<int, 3> a(1, 0, 0), b(0, 1, 0), c(0, 0, 1);
  result = cross(a - b, b) + cross(b, c + b) * 2;
  EXPECT_EQ(2, result[0]);
  EXPECT_EQ(1, std::get<2>(result));
}

TEST(VecTest, accessor) {
  Vec<int, 3> a(1, 2, 3);
  EXPECT_EQ(1, a.x());
  EXPECT_EQ(2, a.y());
  EXPECT_EQ(3, a.z());
  a.x() = -1;
  EXPECT_EQ(-1, a.x());

  [](const Vec<int, 3>& b) {
    EXPECT_EQ(2, b.y());
  }(a);
}
