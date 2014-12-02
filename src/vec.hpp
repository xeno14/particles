/**
 * \file vec.hpp
 *
 *
 */

#pragma once

#include "expression.hpp"
#include <array>

namespace particles {

template <class T, std::size_t N>
class Vec {
 public:
  typedef T value_type;

  Vec() : value_() {}

  T& operator[](std::size_t i) { return value_[i]; }
  const T& operator[](std::size_t i) const { return value_[i]; }

  template <class E>
  Vec& operator=(const E& r) {
    // TODO: Use template to expand
    for (std::size_t i = 0; i < N; i++) (*this)[i] = r[i];
    return *this;
  }

 private:
  std::array<T, N> value_;
};

}  // namespace particles
