/**
 * @file particle.hpp
 *
 * @brief particle: pack of position, velocity.
 */

#pragma once

#include "vec.hpp"

#include <initializer_list>
#include <functional>

template <bool B, class T=void>
using enable_if = typename std::enable_if<B, T>::type*&;

namespace {
extern void* enabler;
}

namespace particles {
namespace internal {

/**
 * @brief info to be held in Particle
 * 
 * If type of info is void, this struct is empty to save memory.
 * @tparam T type of info
 */
template <class T>
struct Info {
  T val;
};

template <>
struct Info<void> {};

}  // namespace internal

/**
 * @brief particle moving in N dimension with info
 *
 * @todo construct with info
 *
 * @tparam T floating point
 * @tparam N dimension
 * @tparam I info of arbitrary type
 */
template <class T, std::size_t N, class I=void>
class Particle {
 public:
  typedef T value_type;
  static constexpr auto DIM = N;

  Particle() : position_(), velocity_() {}

  Particle(const Vec<T, N>& x, const Vec<T, N>& v)
      : position_(x), velocity_(v) {}

  /** @brief specify position using initializer_list 
   *  @todo unable to construct using emplace_back (see searcher_test.cpp:21)
   */
  Particle(std::initializer_list<T> init_list)
      : position_(init_list), velocity_() {}

  /** @brief specify position and velocity using initializer_list */
  Particle(std::initializer_list<T> pos_init, std::initializer_list<T> vel_init)
      : position_(pos_init), velocity_(vel_init) {}

  Vec<T, N>& position() { return position_; }
  Vec<T, N>& velocity() { return velocity_; }
  const Vec<T, N>& position() const { return position_; }
  const Vec<T, N>& velocity() const { return velocity_; }

  /** @brief returns i-th element of position */
  T& position(std::size_t i) { return position_[i]; }
  /** @brief returns i-th element of velocity */
  T& velocity(std::size_t i) { return velocity_[i]; }
  /** @brief returns i-th element of position (const) */
  const T& position(std::size_t i) const { return position_[i]; }
  /** @brief returns i-th element of velocity (const) */
  const T& velocity(std::size_t i) const { return velocity_[i]; }

  T squared_distance(const Particle<T, N>& p) const {
    return position().squared_distance(p.position());
  }
  T squared_distance(const Particle<T, N>* p) const {
    return position().squared_distance(p->position());
  }

  /** @brief dimension of vector */
  static constexpr std::size_t dim() { return N; }

  /** @brief dummy accessor for info if its type is void */
  template <class U=I, enable_if< std::is_void<U>{}> = enabler>
  void info() const {}

  /** @brief accessor for info */
  template <class U=I, enable_if<!std::is_void<U>{}> = enabler>
  auto& info() { return info_.val; }

  /** @brief accessor for info */
  template <class U=I, enable_if<!std::is_void<U>{}> = enabler>
  const auto& info() const { return info_.val; }

 private:
  Vec<T, N> position_;
  Vec<T, N> velocity_;
  internal::Info<I> info_;
};

template <class T, std::size_t N>
T squared_distance(const Particle<T, N>& p, const Particle<T, N>& q) {
  return p.position().squared_distance(q.position());
}
template <class T, std::size_t N>
T squared_distance(const Particle<T, N>* p, const Particle<T, N>* q) {
  return p->position().squared_distance(q->position);
}

}  // namespace particles
