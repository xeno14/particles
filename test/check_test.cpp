#include <gtest/gtest.h>

#include "particles/check.hpp"

#include <array>
#include <tuple>
#include <vector>

using namespace particles;
using namespace particles::check;

TEST(check_test, conditional) {
  static_assert(
      std::is_same<std::true_type, internal::Conditional<true>::type>::value,
      "true_type");
  static_assert(
      std::is_same<std::false_type, internal::Conditional<false>::type>::value,
      "false");
}

TEST(check, is_get_overloaded) {
  static_assert(!is_get_overloaded<int>{}, "");
  static_assert( is_get_overloaded<std::tuple<int>>{}, "");
  static_assert( is_get_overloaded<std::array<int, 100>>{}, "");
}

TEST(check, has_tuple_size) {
  static_assert(!has_tuple_size<int>{}, "");
  static_assert( has_tuple_size<std::tuple<int>>{}, "");
  static_assert( has_tuple_size<std::array<int, 100>>{}, "");
}

TEST(check, is_tuple_like) {
  static_assert(!is_tuple_like<int>{}, "");
  static_assert( is_tuple_like<std::tuple<int>>{}, "");
  static_assert( is_tuple_like<std::array<int, 100>>{}, "");
}

TEST(check, has_access_operator) {
  static_assert(!has_access_operator<int>{}, "");
  static_assert( has_access_operator<std::vector<int>>{}, "");
  static_assert( has_access_operator<std::array<int, 100>>{}, "");
}

TEST(check, has_same_value_type) {
  std::vector<int> u, v;
  std::vector<double> w;

  static_assert( has_same_value_type<decltype(u), decltype(v)>{}, "");
  static_assert(!has_same_value_type<decltype(u), decltype(v), decltype(w)>{},
                "");
  // error
  // auto x = has_same_value_type<decltype(u)>{};
}

TEST(check, has_same_iterator) {
  typedef std::vector<int> vi;
  typedef std::vector<float> vf;
  EXPECT_TRUE((has_same_iterator<vi, vi, vi>{}));
  EXPECT_FALSE((has_same_iterator<vi, vf, vi>{}));
}
