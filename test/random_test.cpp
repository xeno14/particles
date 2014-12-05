#include "random.hpp"
#include "vec.hpp"

#include <gtest/gtest.h>

using particles::random::UniformRand;

const int NUM_TRY = 1000;

TEST(RandomTest, UniformRand) {
  // Default: x in [0,1) 
  for (int i=0; i<NUM_TRY;i++) {
    auto x = UniformRand<double>::get();
    EXPECT_LE(0, x);
    EXPECT_GT(1, x);
  }
  UniformRand<int>::set_range(0, 10);
  for (int i=0; i<NUM_TRY;i++) {
    auto x = UniformRand<int>::get();
    EXPECT_LE(0, x);
    EXPECT_GE(10, x);
  }
  // Make sure distribution for double is different from for int
  for (int i=0; i<NUM_TRY;i++) {
    auto x = UniformRand<double>::get();
    EXPECT_LE(0, x);
    EXPECT_GT(1, x);
  }
}

TEST(RandomTest, get_vec) {
  using particles::Vec;

  Vec<double, 3> v;

  UniformRand<double>::set_range(-1, 1);
  for(int i=0; i<NUM_TRY; i++) {
    v = UniformRand<double>::get_vec();
    for (auto x : v) {
      EXPECT_LE(-1, x);
      EXPECT_GT(1, x);
    }
  }

  UniformRand<int>::set_range(0, 10);
  Vec<int, 3> n;
  for(int i=0; i<NUM_TRY; i++) {
    n = UniformRand<int>::get_vec();
    for (auto x : n) {
      EXPECT_LE(0, x);
      EXPECT_GE(10, x);
    }
  }

  Vec<double, 3> u, result;

  u = {1, 2, 3};
  v = {0, 0, 0};
  UniformRand<double>::set_range(0, 1);

  result = u + v + UniformRand<double>::get_vec();
  for(int i=0; i<NUM_TRY; i++) {
    EXPECT_LE(1, result[0]);
    EXPECT_GT(2, result[0]);
    EXPECT_LE(2, result[1]);
    EXPECT_GT(3, result[1]);
    EXPECT_LE(3, result[2]);
    EXPECT_GT(4, result[2]);
  }

  result = u + UniformRand<double>::get_vec() + v;
  for(int i=0; i<NUM_TRY; i++) {
    EXPECT_LE(1, result[0]);
    EXPECT_GT(2, result[0]);
    EXPECT_LE(2, result[1]);
    EXPECT_GT(3, result[1]);
    EXPECT_LE(3, result[2]);
    EXPECT_GT(4, result[2]);
  }
}
