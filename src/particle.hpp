#pragma once

#include "vec.hpp"

namespace particles {

template <class T, std::size_t N>
class Particle {
 public:
  Vec<T, N>& position() { return position_; }
  Vec<T, N>& velocity() { return velocity_; }
  T& mass() { return mass_; }
  const Vec<T, N>& position() const { return position_; }
  const Vec<T, N>& velocity() const { return velocity_; }
  const T& mass() const { return mass_; }

  constexpr std::size_t dim() { return N; }

 private:
  Vec<T, N> position_;
  Vec<T, N> velocity_;
  T mass_;
};

}  // namespace particles
