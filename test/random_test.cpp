#include "particles/io.hpp"
#include "particles/random.hpp"
#include "particles/vec.hpp"

#include <gtest/gtest.h>

#include <fstream>

using namespace particles;
using particles::random::UniformRand;

const int NUM_TRY = 1000;

template <class F>
void loop(F f, int n = 1000) {
  for (int i = 0; i < n; i++) {
    f();
  }
}

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

class UniformGeneratorTest : public ::testing::Test {
 protected:
  UniformGeneratorTest() : gen_int(0, 10), gen_double(0, 1) {}

  virtual void SetUp() {
    gen_int.seed_dev();
    gen_double.seed_dev();
  }

  random::UniformGenerator<int> gen_int;
  random::UniformGenerator<double> gen_double;
};

TEST_F(UniformGeneratorTest, gen_int) {
  loop([&]() {
    EXPECT_GE(10, gen_int());
    EXPECT_LE(0, gen_int());
  });
}

TEST_F(UniformGeneratorTest, gen_double) {
  loop([&]() {
    EXPECT_GT(1, gen_double());
    EXPECT_LE(0, gen_double());
  });
}

TEST_F(UniformGeneratorTest, hoge) {
  loop([&]() {
    Vec<int, 2> v{1, 2};
    v = v + v + gen_int;
    EXPECT_GE(12, v[0]);
    EXPECT_LE(2, v[0]);
    EXPECT_GE(14, v[1]);
    EXPECT_LE(4, v[1]);
  });
}

class UniformOnSphereTest : public ::testing::Test {
 protected:
  virtual void SetUp() { circle.seed_dev(); sphere.seed_dev(); }

  random::UniformOnSphere<double, 2> circle;
  random::UniformOnSphere<double, 3> sphere;
};

TEST_F(UniformOnSphereTest, circle) {
  loop([&]() {
    Vec<double, 2> v;
    v = circle();
    EXPECT_DOUBLE_EQ(1.0, v.length());
  });
}

TEST_F(UniformOnSphereTest, sphere) {
  loop([&]() {
    Vec<double, 3> v;
    v = sphere();
    EXPECT_DOUBLE_EQ(1.0, v.length());
  });
}
