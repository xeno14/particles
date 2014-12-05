/**
 * @file expression.hpp
 *
 * @brief template meta programming liblary
 */

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
namespace operators {

struct Identity {
  constexpr std::size_t operator()(const std::size_t n) { return n; }
};
struct Odd {
  constexpr std::size_t operator()(const std::size_t n) { return n * 2 + 1; }
};
struct Even {
  constexpr std::size_t operator()(const std::size_t n) { return n * 2; }
};

struct Assign {
  template <class L, class R>
  static L& apply(L& l, const R& r) {
    return l = r;
  }
};

struct PreIncrement {
  template <class T>
  static T& apply(T& t) { return ++t; }
};

}  // namespace operators

namespace internal {

template <std::size_t I, class T, class Op>
struct ExpandUnaryOpImpl {
  template <class F>
  static void apply(T& t, F f) {
    constexpr std::size_t i = f(I-1);
    Op::apply(std::get<i>(t));
    ExpandUnaryOpImpl<I-1, T, Op>::apply(t, f);
  }
};

template <class T, class Op>
struct ExpandUnaryOpImpl<0, T, Op> {
  template <class F>
  static void apply(T& t, F f) { return; }
};

/**
 * Execute assigning at index I-1.
 *
 * @brief expand assigning operators
 * @pre FL and FR have constexpr method named get
 * @tparam I index of elements
 * @tparam L substitute to
 * @tparam R substitute from
 */
template <std::size_t I, class L, class Op, class R>
struct ExpandBinaryOpImpl {
  template <class FL, class FR>
  static void apply(L& l, const R& r, FL fl, FR fr) {
    constexpr std::size_t il = fl(I - 1);
    constexpr std::size_t ir = fr(I - 1);
    Op::apply(std::get<il>(l), std::get<ir>(r));
    ExpandBinaryOpImpl<I - 1, L, Op, R>::apply(l, r, fl, fr);
  }
};

template <class L, class Op, class R>
struct ExpandBinaryOpImpl<0, L, Op, R> {
  template <class FL, class FR>
  static void apply(L& l, const R& r, FL fl, FR fr) { return; }
};

}  // namespace internal

/** @brief pre-increment all elements */
template <std::size_t N, class T, class F=operators::Identity>
inline void pre_increment(T& t, F f=operators::Identity()) {
  typedef operators::PreIncrement Op;
  internal::ExpandUnaryOpImpl<N, T, Op>::apply(t, f);
}

/** @brief pre_increment elements of even index */
template <std::size_t N, class T>
inline void pre_increment_even(T& t) {
  typedef operators::PreIncrement Op;
  internal::ExpandUnaryOpImpl<N, T, Op>::apply(t, operators::Even());
}

/** @brief pre_increment elements of odd index */
template <std::size_t N, class T>
inline void pre_increment_odd(T& t) {
  typedef operators::PreIncrement Op;
  internal::ExpandUnaryOpImpl<N, T, Op>::apply(t, operators::Odd());
}

/** @brief assign [0, 1, ..., N-1] <- [0, 1, ..., N-1] */
template <std::size_t N, class L, class R>
inline void assign(L& l, const R& r) {
  typedef operators::Assign Op;
  internal::ExpandBinaryOpImpl<N, L, Op, R>::apply(l, r, operators::Identity(),
                                                   operators::Identity());
}

/** @brief assign [0, 1, ..., N-1] <- [0, 2, ..., 2N-2] */
template <std::size_t N, class L, class R>
inline void assign_from_even(L& l, const R& r) {
  typedef operators::Assign Op;
  internal::ExpandBinaryOpImpl<N, L, Op, R>::apply(l, r, operators::Identity(),
                                                   operators::Even());
}

/** @brief assign [0, 1, ..., N-1] <- [1, 3, ..., 2N-1] */
template <std::size_t N, class L, class R>
inline void assign_from_odd(L& l, const R& r) {
  typedef operators::Assign Op;
  internal::ExpandBinaryOpImpl<N, L, Op, R>::apply(l, r, operators::Identity(),
                                                   operators::Odd());
}

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
