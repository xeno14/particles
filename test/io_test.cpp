#include "particles/io.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>
#include <vector>

using namespace particles;

class IOTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    ss.str("");
    ss.clear(std::stringstream::goodbit);
  }
  std::stringstream ss;
};

TEST_F(IOTest, output_1) {
  auto t = std::make_tuple(1, 2, 3);
  io::output<3>(ss, t);
  EXPECT_EQ("1 2 3", ss.str());
}

TEST_F(IOTest, output_2) {
  auto t = std::make_tuple(1, 2, 3);
  io::output<3>(ss, t, "  ");
  EXPECT_EQ("1  2  3", ss.str());
}

TEST_F(IOTest, output_3) {
  auto t = std::make_tuple(1, 2, 3);
  io::output<3>(ss, t, ",", "(", ")");
  EXPECT_EQ("(1,2,3)", ss.str());
}

TEST_F(IOTest, output_vec) {
  Vec<int, 3> v {1, 2, 3};
  io::output<3>(ss, v);
  EXPECT_EQ("1 2 3", ss.str());
}

TEST_F(IOTest, output_particle) {
  Particle<int, 2> p({1,2},{3,4});
  io::output_particle(ss, p, ",");
  EXPECT_EQ("1,2,3,4", ss.str());
}

TEST_F(IOTest, Vec_ostream){
  Vec<int, 3> v {1, 2, 3};
  ss << v << "@";
  EXPECT_EQ("[1, 2, 3]@", ss.str());
}

class OutputParticlesTest : public IOTest {
 protected:
  virtual void SetUp() {
    IOTest::SetUp();
    v.push_back(Particle<int, 2>({1, 2}, {3, 4}));
    v.push_back(Particle<int, 2>({5, 6}, {7, 8}));
  }
  virtual void TearDown() {
    v.clear();
  }
  std::vector<Particle<int, 2>> v;
};

TEST_F(OutputParticlesTest, no_sep) {
  io::output_particles(ss, v.begin(), v.end());
  EXPECT_EQ("1 2 3 4\n5 6 7 8\n", ss.str());
}

TEST_F(OutputParticlesTest, with_sep) {
  io::output_particles(ss, v.begin(), v.end(), ",", "@") << "*";
  EXPECT_EQ("1,2,3,4@5,6,7,8@*", ss.str());
}

TEST_F(IOTest, Particle_ostream) {
  Particle<int, 2> p({1,2},{3,4});
  ss << p << "@";
  EXPECT_EQ("{[1, 2], [3, 4]}@", ss.str());
}

TEST_F(IOTest, vec_istream) {
  Vec<int, 3> v;
  int n;

  ss << "1 2 3 4";
  ss >> v >> n;
  EXPECT_EQ(1, v[0]);
  EXPECT_EQ(2, v[1]);
  EXPECT_EQ(3, v[2]);
  EXPECT_EQ(4, n);
}

TEST_F(IOTest, vec_istream2) {
  Vec<int, 2> u;
  Vec<int, 3> v;

  ss << "1 2 3 4 5";
  ss >> u >> v;
  EXPECT_EQ(1, u[0]);
  EXPECT_EQ(2, u[1]);
  EXPECT_EQ(3, v[0]);
  EXPECT_EQ(4, v[1]);
  EXPECT_EQ(5, v[2]);
}
