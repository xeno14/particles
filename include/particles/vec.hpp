/**
 * \file vec.hpp
 *
 * @todo add examples
 * @todo multiply and division using expression template
 */

#pragma once

#include "expression.hpp"
#include "util.hpp"

#include <array>
#include <cmath>
#include <initializer_list>
#include <ostream>

namespace particles {

/**
 * @brief N-d vector in cartesian
 * @tparam T floating point
 * @tparam N dimension
 * @todo output to ostream
 */
template <class T, std::size_t N>
class Vec {
 public:
  typedef T value_type;
  typedef std::array<T, N> array_t;

  Vec() : value_() { value_.fill(0); }
  Vec(std::initializer_list<T> init_list);
  Vec(const Vec& v) : value_(v.value_) {}
  template <class... Args>
  Vec(Args... args) { expression::assign(value_, args...); }
  template <class E>
  Vec(const E& r) { for(std::size_t i=0; i<N; i++) value_[i] = r[i]; }

  T& operator[](std::size_t i) { return value_[i]; }
  T& operator()(std::size_t i) { return (*this)[i]; }
  const T& operator[](std::size_t i) const { return value_[i]; }
  const T& operator()(std::size_t i) const { return (*this)[i]; }
  template <std::size_t I>
  /** @brief access I-th element using std::get */
  T& get() { return std::get<I>(value_); }
  /** @brief access I-th element using std::get */
  template <std::size_t I>
  const T& get() const { return std::get<I>(value_); }
  void fill(const T val) { value_.fill(val); }

  template <class E>
  Vec& operator=(const E& r);
  template <class E>
  bool operator==(const E& r) const;
  template <class E>
  bool operator!=(const E& r) const;
  template <class E>
  Vec& operator+=(const E& v);
  template <class E>
  Vec& operator-=(const E& v);
  Vec& operator*=(T x);
  Vec& operator/=(T x);

  template <class R>
  auto operator+(const R& r) const {
    typedef typename R::value_type value_type;
    typedef expression::Plus<value_type> Op;
    typedef expression::Exp<Vec, Op, R> Exp;
    return Exp(*this, r);
  }

  template <class R>
  auto operator-(const R& r) const {
    typedef typename R::value_type value_type;
    typedef expression::Minus<value_type> Op;
    typedef expression::Exp<Vec, Op, R> Exp;
    return Exp(*this, r);
  }

  auto operator*(T x) const {
    typedef expression::Multiply<T> Op;
    typedef expression::Scalar<T> Scalar;
    typedef expression::Exp<Vec, Op, Scalar> Exp;
    return Exp(*this, Scalar(x));
  }

  auto operator/(T x) const {
    typedef expression::Divide<T> Op;
    typedef expression::Scalar<T> Scalar;
    typedef expression::Exp<Vec, Op, Scalar> Exp;
    return Exp(*this, Scalar(x));
  }

  // Iterators
  /** @toro range test */
  auto begin() { return value_.begin(); }
  auto end() { return value_.end(); }
  auto cbegin() const { return value_.cbegin(); }
  auto cend() const { return value_.cend(); }
  auto rbegin() { return value_.rbegin(); }
  auto rend() { return value_.rend(); }
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
template <class E>
inline Vec<T, N>& Vec<T, N>::operator+=(const E& r) {
  for (std::size_t i = 0; i < N; ++i) (*this)[i] += r[i];
  return *this;
}

template <class T, std::size_t N>
template <class E>
inline Vec<T, N>& Vec<T, N>::operator-=(const E& r) {
  for (std::size_t i = 0; i < N; ++i) (*this)[i] -= r[i];
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

/**
 * @brief cross product for 3d vectors
 */
template <class T>
inline Vec<T, 3> cross(const Vec<T, 3>& u, const Vec<T, 3>& v) {
  return {
    u[1] * v[2] - u[2] * v[1],
    u[2] * v[0] - u[0] * v[2],
    u[0] * v[1] - u[1] * v[0],
  };
}

}  // namespace particles


namespace std {
using namespace particles;

template <size_t I, class T, size_t N>
inline T& get(particles::Vec<T, N>& v) noexcept {
  return v.get<I>();
}

template <size_t I, class T, size_t N>
const inline T& get(const particles::Vec<T, N>& v) noexcept {
  return v.get<I>();
}

}  // namespace std

using namespace particles;
OVERLOAD_STD_BEGIN_AND_END(class... T, Vec<T...>);
