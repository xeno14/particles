/**
 * @file boundary.hpp
 *
 * @brief boundary contidions for particles
 */

#pragma once

#include "vec.hpp"
#include "particle.hpp"
#include "range.hpp"

#include <algorithm>
#include <cmath>

namespace particles {
namespace boundary {

namespace internal {

/**
 *
 * @param it iterator pointing Particle
 */
template<class Iterator>
auto position_iterator(Iterator it) {
  static auto cvt = [](decltype(*it)& p) { return p.position(); };
  return ::particles::range::convert_iterator(it, cvt);
}

}  // namespace internal


/**
 * @brief apply periodic boundary condition to I-th dimension
 * @tparam T
 * @tparam I index of elements to apply condition
 */
template <class T, std::size_t I>
class PeriodicBoundary {
 public:
  PeriodicBoundary(T a, T b) : a_(std::min<T>(a, b)), b_(std::max<T>(a, b)) {}

  /**
   * @brief apply periodic condition
   * @pre std::get must be available for type Tuple.
   * @tparam Tuple 
   */
  template <class Tuple>
  void apply(Tuple& p) {
    const T L = b_ - a_;
    auto& x = std::get<I>(p);

    // Move left to 0, say [0, L)
    x -= a_;
    // Move x inside [0, L)
    x = x - L * std::floor(x / L);
    // Move original plaece
    x += a_;
  }

 private:
  const T a_, b_;
};

}  // namespace boundary
}  // namespace particles
