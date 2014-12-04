#pragma once

#include "vec.hpp"

#include <initializer_list>

namespace particles {

template <class T, std::size_t N>
class Particle {
 public:
  Particle(std::initializer_list<T> init_list)
      : position_(init_list), velocity_(), mass_(1) {}
  Particle(const Vec<T, N>& x, const Vec<T, N>& v, T m=1)
      : position_(x), velocity_(v), mass_(m) {}

  Vec<T, N>& position() { return position_; }
  Vec<T, N>& velocity() { return velocity_; }
  Vec<T, N>& position(std::size_t i) { return position_(i); }
  Vec<T, N>& velocity(std::size_t i) { return velocity_(i); }
  T& mass() { return mass_; }
  const Vec<T, N>& position() const { return position_; }
  const Vec<T, N>& velocity() const { return velocity_; }
  const Vec<T, N>& position(std::size_t i) const { return position_(i); }
  const Vec<T, N>& velocity(std::size_t i) const { return velocity_(i); }
  const T& mass() const { return mass_; }

  constexpr std::size_t dim() { return N; }

 private:
  Vec<T, N> position_;
  Vec<T, N> velocity_;
  T mass_;
};

}  // namespace particles
