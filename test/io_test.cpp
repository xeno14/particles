#include "io.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <tuple>

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

TEST(IoTest, ostream) {
  std::stringstream ss;

  Vec<int, 3> v {1, 2, 3};

  ss << v;
  EXPECT_EQ("[1, 2, 3]", ss.str());
  clear_ss(ss);
}
