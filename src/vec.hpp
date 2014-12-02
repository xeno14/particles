/**
 * \file vec.hpp
 *
 *
 */

#pragma once

#include "expression.hpp"
#include <array>
#include <initializer_list>

namespace particles {

template <class T, std::size_t N>
class Vec {
 public:
  typedef T value_type;

  Vec() : value_() {}
  Vec(constexpr std::initializer_list<T> init_list);

  T& operator[](std::size_t i) { return value_[i]; }
  const T& operator[](std::size_t i) const { return value_[i]; }

  template <class E>
  Vec& operator=(const E& r) {
    // TODO: Use template to expand this loop
    for (std::size_t i = 0; i < N; i++) (*this)[i] = r[i];
    return *this;
  }

 private:
  std::array<T, N> value_;
};

template <class T, std::size_t N>
Vec<T, N>::Vec(constexpr std::initializer_list<T> init_list) {
  static_assert(N == init_list.size(), "size");
  // TODO: size check
  auto it = value_.begin();
  for (auto x : init_list) *(it++) = x;
}

}  // namespace particles
