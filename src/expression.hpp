#pragma once

#include <tuple>
#include <cstdlib>
#include <iostream>

namespace particles {
namespace expression {
template <class L, class Op, class R>
struct Expression;
}
}

namespace std {

template <size_t I, class L, class Op, class R>
inline auto& get(particles::expression::Expression<L, Op, R>& e) {
  return Op::apply(get<I>(e.l), get<I>(e.r));
}

}  // namespace std

namespace particles {

namespace expression {

/** @brief N */
template <std::size_t N>
struct Identity {
  static constexpr std::size_t value = N;
};

/**
 * @brief 2N+1
 */
template <std::size_t N>
struct Odd {
  static constexpr std::size_t value = 2 * N + 1;
};

/**
 * @brief 2N
 */
template <std::size_t N>
struct Even {
  static constexpr std::size_t value = 2 * N;
};


/**
 * @brief expand assigning operators
 * @tparam N num of elements
 * @tparam L substitute to
 * @tparam R substitute from
 * @tparam Opl N -> ?, such as Identity, Odd, Even
 * @tparam Opr N -> ?, such as Identity, Odd, Even
 */
template <std::size_t N, class L, class R, class Opl = Identity<N>,
          class Opr = Identity<N>>
struct Assign {
  static void apply(L& l, const R& r) {
    std::get<Opl::value>(l) = std::get<Opr::value>(r);
    Assign<N - 1, L, R, Opl, Opr>(l, r);
  }
};

template <class L, class R, class Opl, class Opr>
struct Assign<0, L, R, Opl, Opr> {
  static void apply(L& l, const R& r) {
    std::get<Opl::value>(l) = std::get<Opr::value>(r);
  }
};

template <class L, class Op, class R>
struct Expression {
  const L& l;
  const R& r;

  Expression(const L& l, const R& r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }
};

template <class L, class Op, class R>
struct ExpressionRCopy {
  const L& l;
  const R r;

  ExpressionRCopy(const L& l, const R& r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }
};


//
// Operators
//
template <class T>
struct Plus {
  static T apply(T l, T r) { return l + r; }
};

template <class T>
struct Minus {
  static T apply(T l, T r) { return l - r; }
};

template <class T>
struct Multiply {
  static T apply(T l, T r) { return l * r; }
};

template <class T>
struct Divide {
  static T apply(T l, T r) { return l / r; }
};

/**
 * Scalar
 * 
 * Used for multiply and division.
 */
template <class T>
struct Scalar {
  T value;
  Scalar(T value) : value(value) {}
  T operator[](std::size_t i) const { return value; }
};

}  // namespace expression

using expression::Expression;
using expression::ExpressionRCopy;
using expression::Plus;
using expression::Minus;
using expression::Multiply;
using expression::Divide;

template <class L, class R>
inline auto operator+(const L& l, const R& r) {
  typedef typename R::value_type value_type;
  return Expression<L, Plus<value_type>, R>(l, r);
}

template <class L, class R>
inline auto operator-(const L& l, const R& r) {
  typedef typename R::value_type value_type;
  return Expression<L, Minus<value_type>, R>(l, r);
}

template <class L, class T>
inline auto operator*(const L& l, T r) {
  typedef typename L::value_type value_type;
  typedef expression::Scalar<value_type> Scalar;
  return ExpressionRCopy<L, Multiply<value_type>, Scalar>(l, Scalar(r));
}

template <class L, class T>
inline auto operator/(const L& l, T r) {
  typedef typename L::value_type value_type;
  typedef expression::Scalar<value_type> Scalar;
  return ExpressionRCopy<L, Divide<value_type>, Scalar>(l, Scalar(r));
}

}  // namespace particles
