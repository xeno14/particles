#include "random.hpp"
#include "vec.hpp"

#include <gtest/gtest.h>

using particles::random::UniformRand;

const int NUM_TRY = 1000;

TEST(RandomTest, UniformRand) {
  // Default: x in [0,1) 
  for (int i=0; i<NUM_TRY;i++) {
    auto x = UniformRand<double>::get();
    EXPECT_TRUE(0 <= x && x < 1);
  }
  UniformRand<double>::set_range(0, 100);
  for (int i=0; i<NUM_TRY;i++) {
    auto x = UniformRand<double>::get();
    EXPECT_TRUE(0 <= x && x < 100);
  }
  UniformRand<int>::set_range(0, 10);
  for (int i=0; i<NUM_TRY;i++) {
    auto x = UniformRand<int>::get();
    EXPECT_TRUE(0 <= x && x <= 10);
  }
}

TEST(RandomTest, get_vec) {
  using particles::Vec;

  Vec<double, 3> v;

  UniformRand<double>::set_range(-1, 1);
  for(int i=0; i<NUM_TRY; i++) {
    v = UniformRand<double>::get_vec();
    for (auto x : v) EXPECT_TRUE(-1 <= x && x < 1);
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
}
