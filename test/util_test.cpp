#include "particles/util.hpp"

#include <sstream>
#include <string>
#include <fstream>

#include <gtest/gtest.h>

using namespace particles;

TEST(CheckTest, check) {
  // Output nothing
  CHECK(1 == 1) << "one is one\n";
  // Terminate
  // CHECK(1 == 2) << "one is not two\n";
}
