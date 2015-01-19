/**
 * @file expression.hpp
 *
 * @brief template meta programming liblary
 *
 * Consept:
 * - Exp
 * - Index Operator: 
 *   @code
 *   struct IndexOperator {
 *     constexpr std::size_t operator()(std::size_t i) { ... }
 *   };
 *   @endcode
 */

#pragma once

#include <tuple>
#include <cstdlib>
#include <iostream>

namespace particles {
namespace expression {
template <class L, class Op, class R>
struct Exp;
}
}

namespace std {

template <size_t I, class L, class Op, class R>
inline auto& get(particles::expression::Exp<L, Op, R>& e) {
  return Op::apply(get<I>(e.l), get<I>(e.r));
}

}  // namespace std

namespace particles {
namespace expression {
namespace operators {

/**
 * IndexOperator concept
 */
struct Identity {
  constexpr std::size_t operator()(const std::size_t n) { return n; }
};
/**
 * IndexOperator concept
 */
struct Odd {
  constexpr std::size_t operator()(const std::size_t n) { return n * 2 + 1; }
};
/**
 * IndexOperator concept
 */
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

/**
 * @brief pre-increment all elements 
 * @tparam N num of elements
 * @tparam T type
 * @tparam F index operator concept
 * @todo skip odd and skip even
 */
template <std::size_t N, class T, class F=operators::Identity>
inline void pre_increment(T& t, F f=operators::Identity()) {
  typedef operators::PreIncrement Op;
  internal::ExpandUnaryOpImpl<N, T, Op>::apply(t, f);
}

/** @brief assign [0, 1, ..., N-1] <- [0, 1, ..., N-1] */
template <std::size_t N, class L, class R, class FL = operators::Identity,
          class FR = operators::Identity>
inline void assign(L& l, const R& r, FL fl = operators::Identity(),
                   FR fr = operators::Identity()) {
  typedef operators::Assign Op;
  internal::ExpandBinaryOpImpl<N, L, Op, R>::apply(l, r, fl, fr);
}

/** @brief assign [0, 1, ..., N-1] <- [0, 2, ..., 2N-2] */
template <std::size_t N, class L, class R>
inline void assign_from_even(L& l, const R& r) {
  assign<N, L, R>(l, r, operators::Identity(), operators::Even());
}

/** @brief assign [0, 1, ..., N-1] <- [1, 3, ..., 2N-1] */
template <std::size_t N, class L, class R>
inline void assign_from_odd(L& l, const R& r) {
  assign<N, L, R>(l, r, operators::Identity(), operators::Odd());
}

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
  Scalar(T x) : value(x) {}
  T operator[](std::size_t i) const { return value; }
};

/**
 * @brief Expression!
 *
 * Each operators return this object instead of value itself. As a result,
 * expression will be evaluated at assigning operator.
 */
template <class L, class Op, class R>
struct Exp {
  typedef R value_type;
  const L& l;
  const R& r;

  Exp(const L& l, const R& r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }

  template <class R2>
  auto operator+(const R2& r2) {
    typedef typename R2::value_type value_type;
    return Exp<Exp<L, Op, R>, Plus<value_type>, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    typedef typename R2::value_type value_type;
    return Exp<Exp<L, Op, R>, Minus<value_type>, R2>(*this, r2);
  }

  template <class T>
  auto operator*(const Scalar<T>& s) {
    return Exp<Exp<L, Op, R>, Multiply<T>, Scalar<T>>(*this, s);
  }

  template <class T>
  auto operator/(const Scalar<T>& s) {
    return Exp<Exp<L, Op, R>, Divide<value_type>, Scalar<T>>(*this, s);
  }
};

/**
 * @brief copy the right side for scalar
 */
template <class L, class Op, class T>
struct Exp<L, Op, Scalar<T>> {
  typedef T value_type;
  typedef Scalar<T> R;
  const L& l;
  const R r;

  Exp(const L& l, const R& r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }

  template <class R2>
  auto operator+(const R2& r2) {
    typedef typename R2::value_type value_type;
    return Exp<Exp<L, Op, R>, Plus<value_type>, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    typedef typename R2::value_type value_type;
    return Exp<Exp<L, Op, R>, Minus<value_type>, R2>(*this, r2);
  }

  template <class U>
  auto operator*(const Scalar<U>& s) {
    return Exp<Exp<L, Op, R>, Multiply<U>, Scalar<U>>(*this, s);
  }

  template <class U>
  auto operator/(const Scalar<U>& s) {
    return Exp<Exp<L, Op, R>, Divide<U>, Scalar<U>>(*this, s);
  }
};

}  // namespace expression

}  // namespace particles