/**
 * \file vec.hpp
 *
 *
 */

#pragma once

#include "expression.hpp"

#include <array>
#include <cmath>
#include <initializer_list>
#include <ostream>

#include <iostream>

namespace particles {

/**
 * @brief N-d vector in cartesian
 * @tparam T floating point
 * @tparam N dimension
 */
template <class T, std::size_t N>
class Vec {
 public:
  typedef T value_type;
  typedef std::array<T, N> array_t;

  Vec() : value_() { value_.fill(0); }
  Vec(std::initializer_list<T> init_list);
  Vec(const Vec& v) : value_(v.value_) {}

  T& operator[](std::size_t i) { return value_[i]; }
  T& operator()(std::size_t i) { return (*this)[i]; }
  const T& operator[](std::size_t i) const { return value_[i]; }
  const T& operator()(std::size_t i) const { return (*this)[i]; }
  template <std::size_t I>
  /** @brief access I-th element using std::get */
  T& get() { return std::get<I>(value_); }
  void fill(const T val) { value_.fill(val); }

  template <class E>
  Vec& operator=(const E& r);
  template <class E>
  bool operator==(const E& r) const;
  template <class E>
  bool operator!=(const E& r) const;
  Vec& operator+=(const Vec& v);
  Vec& operator-=(const Vec& v);
  Vec& operator*=(T x);
  Vec& operator/=(T x);

  // Iterators
  auto begin() { value_.begin(); }
  auto end() { value_.end(); }
  auto cbegin() const { return value_.cbegin(); }
  auto cend() const { return value_.cend(); }
  auto rbegin() { value_.rbegin(); }
  auto rend() { value_.rend(); }
  auto crbegin() const { return value_.crbegin(); }
  auto crend() const { return value_.crend(); }

  // Mathematical functions
  static const Vec<T, N>& zero() { static Vec<T, N> z; return z; }

  T squared_distance(const Vec& v) const;
  T squared_length() const;
  T distance(const Vec& v) const;
  T length() const;
  T dot(const Vec& v) const;
  /** @brief normalize length of this vector to specifed value */
  Vec& normalize(T len=1);
  /** 
   * @brief check this condition: \f$ |\vec{u}||\vec{v}| - \vec{u} \cdot \vec{v} < \epsilon\f$
   * @param v vector
   * @param eps threshold
   */
  bool parallel(const Vec& v, T eps=1e-8) const;

 private:
  array_t value_;
};

template <class T, std::size_t N>
Vec<T, N>::Vec(std::initializer_list<T> init_list) {
  /** @todo size check */
  auto it = value_.begin();
  for (auto x : init_list) *(it++) = x;
}

template <class T, std::size_t N>
template <class E>
inline Vec<T, N>& Vec<T, N>::operator=(const E& r) {
  /** @todo Use template to expand this loop */
  // expression::Assign<N, array_t, E>::apply(value_, r);
  for (std::size_t i = 0; i < N; i++) (*this)[i] = r[i];
  return *this;
}

template <class T, std::size_t N>
template <class E>
inline bool Vec<T, N>::operator==(const E& r) const {
  for (std::size_t i = 0; i < N; i++) if(!((*this)[i] == r[i])) return false;
  return true;
}

template <class T, std::size_t N>
template <class E>
inline bool Vec<T, N>::operator!=(const E& r) const {
  return !(*this == r);
}

template <class T, std::size_t N>
inline Vec<T, N>& Vec<T, N>::operator+=(const Vec<T, N>& v) {
  for (std::size_t i = 0; i < N; ++i) (*this)[i] += v[i];
  return *this;
}

template <class T, std::size_t N>
inline Vec<T, N>& Vec<T, N>::operator-=(const Vec<T, N>& v) {
  for (std::size_t i = 0; i < N; ++i) (*this)[i] -= v[i];
  return *this;
}

template <class T, std::size_t N>
inline Vec<T, N>& Vec<T, N>::operator*=(T x) {
  for (std::size_t i = 0; i < N; ++i) (*this)[i] *= x;
  return *this;
}

template <class T, std::size_t N>
inline Vec<T, N>& Vec<T, N>::operator/=(T x) {
  for (std::size_t i = 0; i < N; ++i) (*this)[i] /= x;
  return *this;
}

template <class T, std::size_t N>
inline T Vec<T, N>::squared_distance(const Vec<T, N>& v) const {
  T d = 0;
  for (std::size_t i = 0; i < N; ++i)
    d += ((*this)[i] - v[i]) * ((*this)[i] - v[i]);
  return d;
}

template <class T, std::size_t N>
inline T Vec<T, N>::squared_length() const {
  return Vec<T, N>::squared_distance(Vec<T, N>::zero());
}

template <class T, std::size_t N>
inline T Vec<T, N>::distance(const Vec<T, N>& v) const {
  return std::sqrt(squared_distance(v));
}

template <class T, std::size_t N>
inline T Vec<T, N>::length() const {
  return std::sqrt(squared_length());
}

template <class T, std::size_t N>
inline T Vec<T, N>::dot(const Vec<T, N>& v) const {
  T d = 0;
  for (std::size_t i = 0; i < N; ++i) d += (*this)[i] * v[i];
  return d;
}

template <class T, std::size_t N>
inline Vec<T,N>& Vec<T, N>::normalize(T len) {
  if (length() == 0) return *this;  // zero vector
  *this /= length();
  return *this *= len;
}

template <class T, std::size_t N>
inline bool Vec<T, N>::parallel(const Vec<T, N>& v, T eps) const {
  return length() * v.length() - dot(v) < eps;
}

}  // namespace particles

namespace std {

template <size_t I, class T, size_t N>
inline T& get(particles::Vec<T, N>& v) noexcept {
  return v.get<I>();
}

template <class T, size_t N>
ostream& operator<<(ostream& os, particles::Vec<T,N>& v) {
  /** @todo expand using template **/
  return os << "(" << v[0] << "," << v[1] << "," << v[2] << ")";
}

}  // namespace std
