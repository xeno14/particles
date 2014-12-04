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

struct Identity {
  constexpr std::size_t operator()(const std::size_t n) { return n; }
};
struct Odd {
  constexpr std::size_t operator()(const std::size_t n) { return n * 2 + 1; }
};
struct Even {
  constexpr std::size_t operator()(const std::size_t n) { return n * 2; }
};

/**
 * Execute assigning at index I-1.
 *
 * @brief expand assigning operators
 * @pre FL and FR have constexpr method named get
 * @tparam I index of elements
 * @tparam L substitute to
 * @tparam R substitute from
 * @tparam FL 
 * @tparam FR
 */
template <std::size_t I, class L, class R>
struct AssignImpl {
  template <class FL, class FR>
  static void apply(L& l, const R& r, FL fl, FR fr) {
    constexpr std::size_t il = fl(I-1);
    constexpr std::size_t ir = fr(I-1);
    std::get<il>(l) = std::get<ir>(r);
    AssignImpl<I-1, L, R>::apply(l, r, fl, fr);
  }
};

template <class L, class R>
struct AssignImpl<0, L, R>  {
  template <class FL, class FR>
  static void apply(L& l, const R& r, FL fl, FR fr) { return ; }
}; 

/** @brief assign [0, 1, ..., N-1] <- [0, 1, ..., N-1] */
template <std::size_t N, class L, class R>
inline void assign(L& l, const R& r) {
  AssignImpl<N, L, R>::apply(l, r, Identity(), Identity());
}

/** @brief assign [0, 1, ..., N-1] <- [0, 2, ..., 2N-2] */
template <std::size_t N, class L, class R>
inline void assign_from_even(L& l, const R& r) {
  AssignImpl<N, L, R>::apply(l, r, Identity(), Even());
}

/** @brief assign [0, 1, ..., N-1] <- [1, 3, ..., 2N-1] */
template <std::size_t N, class L, class R>
inline void assign_from_odd(L& l, const R& r) {
  AssignImpl<N, L, R>::apply(l, r, Identity(), Odd());
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
