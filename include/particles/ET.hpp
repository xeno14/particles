/**
 * @file ET.hpp
 *
 * @brief Expression template
 *
 */

#pragma once

#include "check.hpp"

extern void* enabler;

#define ET_OPERATOR_PLUS(THIS) \
template <class _R2> \
inline auto operator+(const _R2& r2) const { \
  return ET::Exp<THIS, ET::Plus, _R2>(*this, r2); \
} \

#define ET_OPERATOR_MINUS(THIS) \
template <class _R2> \
inline auto operator-(const _R2& r2) const { \
  return ET::Exp<THIS, ET::Minus, _R2>(*this, r2); \
} \

#define ET_OPERATOR_MULTIPLY(THIS) \
template <typename _R2, enabler_if<std::is_arithmetic<_R2>{}> = enabler> \
inline auto operator*(_R2 r2) const { \
  return ET::Exp<THIS, ET::Multiply, ET::Scalar<_R2>>(*this, r2); \
} \

#define ET_OPERATOR_DIVIDE(THIS) \
template <typename _R2, enabler_if<std::is_arithmetic<_R2>{}> = enabler> \
inline auto operator/(_R2 r2) const { \
  return ET::Exp<THIS, ET::Divide, ET::Scalar<_R2>>(*this, r2); \
} \

/**
 * @brief Overload operators + - * /
 *
 * Returns ET::Exp
 */
#define ET_OPERATORS(THIS) \
ET_OPERATOR_PLUS(THIS); \
ET_OPERATOR_MINUS(THIS); \
ET_OPERATOR_MULTIPLY(THIS); \
ET_OPERATOR_DIVIDE(THIS); \

namespace particles {

template<bool B, typename T = void>
using enabler_if = typename std::enable_if<B, T>::type*&;


/**
 * @brief Expression Template
 *
 * Consepts of expression template class:
 *  - has operator[]
 */
namespace ET {

// Operators
struct Plus {
  template <class L, class R>
  static auto apply(L l, R r) { return l + r; }
};

struct Minus {
  template <class L, class R>
  static auto apply(L l, R r) { return l - r; }
};

struct Multiply {
  template <class L, class R>
  static auto apply(L l, R r) { return l * r; }
};

struct Divide {
  template <class L, class R>
  static auto apply(L l, R r) { return l / r; }
};

/**
 * Scalar
 * 
 * Used for multiply and division.
 */
template <class T>
struct Scalar {
  T value;
  Scalar(T x) : value(x) {}
  Scalar(const Scalar<T>& s) : value(s.value) {}
  T operator[](std::size_t i) const { return value; }
};


/**
 * @brief Expression!
 *
 * Operations return Exp class object instead of value itself in order to avoid
 * creating temporary objects. Expression will be evaluated when operator[] is
 * called.
 */
template <class L, class Op, class R>
struct Exp {
  typedef R value_type;
  typedef Exp<L, Op, R> THIS;

  const L& l;
  const R& r;

  Exp(const L& l, const R& r) : l(l), r(r) {}

  /** @brief accessor and apply operation. */
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }

  ET_OPERATORS(THIS);
};


/**
 * @brief Partial specialization for Scalar type
 *
 * Copy the right side for scalar.
 */
template <class L, class Op, class T>
struct Exp<L, Op, Scalar<T>> {
  typedef T value_type;
  typedef Scalar<T> R;
  typedef Exp<L, Op, R> THIS;

  const L& l;
  const R r;

  Exp(const L& l, const R& r) : l(l), r(r) {}
  Exp(const L& l, T r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }

  template <class R2>
  auto operator+(const R2& r2) {
    return Exp<THIS, Plus, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    return Exp<THIS, Minus, R2>(*this, r2);
  }

  template <class U>
  auto operator*(const Scalar<U>& s) {
    return Exp<THIS, Multiply, Scalar<U>>(*this, s);
  }

  template <class U>
  auto operator/(const Scalar<U>& s) {
    return Exp<THIS, Divide, Scalar<U>>(*this, s);
  }
};

#define CROSS_PAIR_FIRST(I)  ((I+1)%3)
#define CROSS_PAIR_SECOND(I) ((I+2)%3)
#define CROSS_SIGN(I, J) (I==J ? 0 : (I+1)%3==J ? 1 : -1)

template <std::size_t I>
struct Pair {
  static constexpr std::size_t first  = CROSS_PAIR_FIRST(I);
  static constexpr std::size_t second = CROSS_PAIR_SECOND(I);
};

template <std::size_t I, std::size_t J>
struct Sign {
  static constexpr int value = CROSS_SIGN(I, J);
};

/** @brief Implementation of multiplying between quaternions
 * @todo implement 8
 */
template <class L, class R>
struct Cross {
  typedef Cross<L, R> THIS;

  const L& l;
  const R& r;

  Cross(const L& l, const R& r) : l(l), r(r) {}

  template <std::size_t I>
  auto get() const {
    static_assert(I < 3, "index is out of bounds");
    constexpr auto J  = Pair<I>::first;
    constexpr auto K  = Pair<I>::second;
    constexpr auto s1 = Sign<J, K>::value;
    constexpr auto s2 = Sign<K, J>::value;

    return std::get<J>(l) * std::get<K>(r) * s1 +
           std::get<K>(l) * std::get<J>(r) * s2;
  }

  void get_pair(std::size_t i, std::size_t& j, std::size_t& k) const {
    j = CROSS_PAIR_FIRST(i); k = CROSS_PAIR_SECOND(i);
  }

  int sign(std::size_t i, std::size_t j) const {
    return CROSS_SIGN(i, j);
  }

  auto operator[] (std::size_t i) const {
    std::size_t j, k;
    get_pair(i, j, k);
    return l[j] * r[k] * sign(j, k) + l[k] * r[j] * sign(k, j);
  }

  ET_OPERATORS(THIS);
};

}  // namespace ET

template <class L, class R>
inline auto cross(const L& l, const R& r) {
  return ET::Cross<L, R>(l, r);
}

}  // namespace particles


namespace std {
using namespace particles;

template <class L, class Op, class R>
struct tuple_size<ET::Exp<L, Op, R>> : public std::enable_if<
    std::tuple_size<L>{} == std::tuple_size<R>{},
    std::integral_constant<std::size_t, std::tuple_size<L>{}>>::type {};

template <class L, class R>
struct tuple_size<ET::Cross<L, R>>
    : public std::integral_constant<std::size_t, 3> {};

template <size_t I, class L, class Op, class R>
inline auto get(const ET::Exp<L, Op, R>& e) {
  static_assert(check::is_get_overloaded<L>{} && check::is_get_overloaded<R>{},
                "Requires that std::get is overloaded");
  return Op::apply(get<I>(e.l), get<I>(e.r));
}

template <size_t I, class L, class R>
inline auto get(const ET::Cross<L, R>& e) {
  return e.get<I>();
}

}  // namespace std
