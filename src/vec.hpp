/**
 * \file vec.hpp
 *
 *
 */

#pragma once

namespace particles {
template <class T = double>
class Vec3 {
 public:
  T x, y, z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
  Vec3(const Vec3<T>& v) { clone(); }
  ~Vec3() {}

  // Operators
  // TODO: Use expression template
  Vec3 operator+(const Vec3& v) const;
  Vec3 operator-(const Vec3& v) const;
  Vec3 operator*(T a) const;
  Vec3 operator/(T a) const;
  Vec3& operator=(const Vec3& v);
  Vec3& operator+=(const Vec3& v);
  Vec3& operator-=(const Vec3& v);
  Vec3& operator*=(T a);
  Vec3& operator/=(T a);

  static const T squared_length(const Vec3& v);
  static const T squared_distace(const Vec3& u, const Vec3 v);
  const T length() const;
  const T distance(const Vec3& v) const;

  static T dot(const Vec3& u, const Vec3& v);

 private:
  Vec3<T>& clone(const Vec3<T>& v);
};

}  // namespace particles
