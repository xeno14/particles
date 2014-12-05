#include "io.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>
#include <vector>

using namespace particles;

void clear_ss(std::stringstream& ss) {
  ss.str("");
  ss.clear(std::stringstream::goodbit);
}

TEST(IoTest, output) {
  std::stringstream ss;
  
  auto t = std::make_tuple(1, 2, 3);
  
  io::output<3>(ss, t);
  EXPECT_EQ("1 2 3", ss.str());
  clear_ss(ss);

  io::output<3>(ss, t, "  ");
  EXPECT_EQ("1  2  3", ss.str());
  clear_ss(ss);

  io::output<3>(ss, t, ",", "(", ")");
  EXPECT_EQ("(1,2,3)", ss.str());
  clear_ss(ss);

  Vec<int, 3> v {1, 2, 3};
  io::output<3>(ss, v);
  EXPECT_EQ("1 2 3", ss.str());
  clear_ss(ss);
}

TEST(IoTest, output_particle) {
  std::stringstream ss;

  Particle<int, 2> p({1,2},{3,4},5);

  io::output_particle(ss, p, ",");
  EXPECT_EQ("1,2,3,4", ss.str());
  clear_ss(ss);
}

TEST(IoTest, Vec_ostream){
  std::stringstream ss;

  Vec<int, 3> v {1, 2, 3};

  ss << v;
  EXPECT_EQ("[1, 2, 3]", ss.str());
  clear_ss(ss);

  ss << v << "@" << v;
  EXPECT_EQ("[1, 2, 3]@[1, 2, 3]", ss.str());
  clear_ss(ss);
}

TEST(IoTest, output_particles) {
  std::stringstream ss;

  std::vector<Particle<int, 2>> v;
  v.push_back(Particle<int, 2>({1, 2}, {3, 4}));
  v.push_back(Particle<int, 2>({5, 6}, {7, 8}));

  io::output_particles(ss, v.begin(), v.end());
  EXPECT_EQ("1 2 3 4\n5 6 7 8\n", ss.str());
  clear_ss(ss);

  io::output_particles(ss, v.begin(), v.end(), ",", "@");
  EXPECT_EQ("1,2,3,4@5,6,7,8@", ss.str());
}

TEST(IoTest, Particle_ostream) {
  std::stringstream ss;

  Particle<int, 2> p({1,2},{3,4},5);

  ss << p;
  EXPECT_EQ("{[1, 2], [3, 4], 5}", ss.str());
  clear_ss(ss);
}
