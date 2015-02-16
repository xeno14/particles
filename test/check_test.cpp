#include <gtest/gtest.h>

#include "particles/check.hpp"

using namespace particles;
using namespace particles::check;

TEST(is_tuple_like, tuple) {
  static_assert(!is_tuple_like<int>{}, "");
  static_assert( is_tuple_like<std::tuple<int>>{}, "");
}
