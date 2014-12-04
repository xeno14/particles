#include "random.hpp"

#include <gtest/gtest.h>

using particles::random::UniformRand;

TEST(RandomTest, UniformRand) {
  // Default: x in [0,1) 
  for (int i=0; i<100;i++) {
    auto x = UniformRand<double>::get();
    EXPECT_TRUE(0 <= x && x < 1);
  }
  UniformRand<double>::set_range(0, 100);
  for (int i=0; i<100;i++) {
    auto x = UniformRand<double>::get();
    EXPECT_TRUE(0 <= x && x < 100);
  }
  UniformRand<int>::set_range(0, 100);
  for (int i=0; i<100;i++) {
    auto x = UniformRand<int>::get();
    EXPECT_TRUE(0 <= x && x <= 100);
  }
}
