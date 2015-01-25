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
#include <tuple>

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
  return ::particles::convert_iterator(it, cvt);
}

/**
 * @todo test fails when a=0, b=1, x=2.2
 *
 * @brief periodic condition for 1d
 * @tparam T value type
 * @param a lower bound
 * @param b upper bound
 */
template <class T>
void apply_periodic_impl(T& x, const T a, const T b) {
  const T L = b - a;
  // Move left to 0, say [0, L)
  x -= a;
  // Move x inside [0, L)
  x = x - L * std::floor(x / L);
  // Move original plaece
  x += a;
}

/**
 * @brief apply periodic condition for each dimensions
 */
template <class T, std::size_t I>
struct PeriodicRectImpl {
  template <class U, class L, class R>
  static void apply(U& u, const L& l, const R& r) {
    internal::apply_periodic_impl<T>(std::get<I - 1>(u), std::get<I - 1>(l),
                                     std::get<I - 1>(r));
    PeriodicRectImpl<T, I-1>::apply(u, l, r);
  }
};

template <class T>
struct PeriodicRectImpl<T, 0> {
  template <class U, class L, class R>
  static void apply(U& u, const L& l, const R& r) { return; }
};

}  // namespace internal

template <class T, std::size_t N>
class BoundaryBase {
 public:
  virtual void apply(Particle<T, N>& p) = 0;
};

/**
 * @brief do nothing
 */
template <class T, std::size_t N>
struct FreeBoundary : public BoundaryBase<T, N> {
  void apply(Particle<T, N>& u) {}
};

/**
 * @brief apply periodic boundary condition to I-th dimension
 *
 * @tparam T
 * @tparam N dimension
 */
template <class T, std::size_t N>
class PeriodicBoundary : public BoundaryBase<T, N> {
 public:
  template <class... Args>
  PeriodicBoundary(Args... args) : left_(), right_() {
    static_assert(sizeof...(args) == N * 2, "number of arguments mismatch");
    std::tuple<Args...> values(args...);
    expression::assign_from_even<N>(left_, values);
    expression::assign_from_odd<N>(right_, values);
  }

  PeriodicBoundary(T L) : left_(), right_() {
    left_.fill(0);
    right_.fill(L);
  }

  void apply(Particle<T, N>& p) {
    internal::PeriodicRectImpl<T, N>::apply(p.position(), left_, right_);
  }

 private:
  std::array<T, N> left_;
  std::array<T, N> right_;
};

}  // namespace boundary
}  // namespace particles
