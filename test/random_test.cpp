#include "particles/io.hpp"
#include "particles/random.hpp"
#include "particles/vec.hpp"

#include <gtest/gtest.h>

#include <fstream>

using namespace particles;
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

  for(int i=0; i<NUM_TRY; i++) {
    result = u + v + UniformRand<double>::get_vec();
    EXPECT_LE(1, result[0]);
    EXPECT_GT(2, result[0]);
    EXPECT_LE(2, result[1]);
    EXPECT_GT(3, result[1]);
    EXPECT_LE(3, result[2]);
    EXPECT_GT(4, result[2]);
  }

  for(int i=0; i<NUM_TRY; i++) {
    result = u + UniformRand<double>::get_vec() + v;
    EXPECT_LE(1, result[0]);
    EXPECT_GT(2, result[0]);
    EXPECT_LE(2, result[1]);
    EXPECT_GT(3, result[1]);
    EXPECT_LE(3, result[2]);
    EXPECT_GT(4, result[2]);
  }
}

TEST(RandomTest, isotoropic2) {
  Vec<double, 2> v;

  // Already checked plotting
  // std::ofstream fout("isotoropic2.dat");
  for(int i=0; i<NUM_TRY; i++) {
    v = random::IsotoropicRand<double, 2>::get_vec(10.0);
    EXPECT_DOUBLE_EQ(v.squared_length(), 10.0 * 10.0);

    // io::output<2>(fout, v);
    // fout << "\n";
  }
}

TEST(RandomTest, isotoropic3) {
  Vec<double, 3> v;

  // Already checked plotting
  // std::ofstream fout("isotoropic3.dat");
  for(int i=0; i<NUM_TRY; i++) {
    v = random::IsotoropicRand<double, 3>::get_vec(10.0);
    EXPECT_DOUBLE_EQ(v.squared_length(), 10.0 * 10.0);

    // io::output<3>(fout, v);
    // fout << "\n";
  }
}
