#include "mathutil.hpp"

#include <gtest/gtest.h>
#include <vector>

using namespace particles;

TEST(MathTest, Euclidean) {
  double a[] = {1, 1};
  double b[] = {4, 5};
  EXPECT_DOUBLE_EQ(5, euclidean_distance(a, a+2, b));

  std::vector<double> u = {1, 1};
  std::vector<double> v = {4, 5};
  EXPECT_DOUBLE_EQ(5, euclidean_distance(u.begin(), u.end(), v.begin()));
}

class GaussSeidelTest : public ::testing::Test {
  protected:
  virtual void SetUp() {
    A.clear();
    b.clear();
    x.clear();
  }

  std::vector<double> A, b, x;
};

TEST_F(GaussSeidelTest, calc1) {
  A = { 1, 0,
        0, 1};
  b = { 2, 3};
  x.resize(2);

  EXPECT_TRUE(gauss_seidel(A, b, x));
  EXPECT_DOUBLE_EQ(2, x[0]);
  EXPECT_DOUBLE_EQ(3, x[1]);
}

TEST_F(GaussSeidelTest, calc2){
  A = { 1, 2,
        3, 4};
  b = { 1, 3};
  x.resize(2);

  EXPECT_TRUE(gauss_seidel(A, b, x, 1e-17));
  EXPECT_DOUBLE_EQ(1, x[0]);
  EXPECT_DOUBLE_EQ(0, x[1]);
}

// orz
TEST_F(GaussSeidelTest, calc3){
  A = { 2, 1,
        3, 4};
  b = { 3, 7};
  x.resize(2);

  EXPECT_TRUE(gauss_seidel(A, b, x));
  EXPECT_DOUBLE_EQ(1, x[0]);
  EXPECT_DOUBLE_EQ(1, x[1]);
}

TEST_F(GaussSeidelTest, calc4){
  A = { 3, 2, 1,
        1, 4, 1,
        2, 2, 5 };
  b = { 10, 12, 21};
  x.resize(3);

  ASSERT_EQ(9, A.size());
  ASSERT_EQ(3, b.size());
  ASSERT_EQ(3, x.size());
  EXPECT_TRUE(gauss_seidel(A, b, x));
  EXPECT_DOUBLE_EQ(1, x[0]);
  EXPECT_DOUBLE_EQ(2, x[1]);
  EXPECT_DOUBLE_EQ(3, x[2]);
}
