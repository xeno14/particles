#include <gtest/gtest.h>

#include "particles/check.hpp"

#include <array>
#include <tuple>
#include <vector>

using namespace particles;
using namespace particles::check;

TEST(check_test, is_tuple_like) {
  static_assert(!is_tuple_like<int>{}, "");
  static_assert( is_tuple_like<std::tuple<int>>{}, "");
  static_assert( is_tuple_like<std::array<int, 100>>{}, "");
}

TEST(check_test, has_access_operator) {
  static_assert(!has_access_operator<int>{}, "");
  static_assert( has_access_operator<std::vector<int>>{}, "");
  static_assert( has_access_operator<std::array<int, 100>>{}, "");
}