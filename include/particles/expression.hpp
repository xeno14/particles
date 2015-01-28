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

#include "decl_overloads.h"

#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <tuple>


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
    return Exp<Exp<L, Op, R>, Plus, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    return Exp<Exp<L, Op, R>, Minus, R2>(*this, r2);
  }

  template <class T>
  auto operator*(T x) {
    return Exp<Exp<L, Op, R>, Multiply, Scalar<T>>(*this, Scalar<T>(x));
  }

  template <class T>
  auto operator/(T x) {
    return Exp<Exp<L, Op, R>, Divide, Scalar<T>>(*this, Scalar<T>(x));
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
    return Exp<Exp<L, Op, R>, Plus, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    return Exp<Exp<L, Op, R>, Minus, R2>(*this, r2);
  }

  template <class U>
  auto operator*(const Scalar<U>& s) {
    return Exp<Exp<L, Op, R>, Multiply, Scalar<U>>(*this, s);
  }

  template <class U>
  auto operator/(const Scalar<U>& s) {
    return Exp<Exp<L, Op, R>, Divide, Scalar<U>>(*this, s);
  }
};


namespace internal {

template <std::size_t I, class L>
void assign_impl(L& l) {}

template <std::size_t I, class L, class T, class... Us>
void assign_impl(L& l, const T& t, const Us&... us) {
  std::get<I>(l) = t;
  assign_impl<I+1, L>(l, us...);
}

}  // namespace internal


/**
 * @brief assign from variable arguments
 *
 * Assign to object std::get is available for
 * @code
 * std::array<int, 3> a;
 * assign(a, 1, 2, 3);
 * @endcode
 */
template <class L, class... Args>
L& assign(L& l, const Args&... args) {
  internal::assign_impl<0>(l, args...);
  return l;
}

namespace internal {

/** @todo prepare both std::get and operator[] */
template <std::size_t I, class L, class R>
struct InnerProdImpl {
  inline static auto apply(const L& l, const R& r) {
    return std::get<I-1>(l) * std::get<I-1>(r) + InnerProdImpl<I-1, L, R>::apply(l, r);
    // return l[I-1] * r[I-1] + InnerProdImpl<I-1, L, R>::apply(l, r);
  }
};

template <class L, class R>
struct InnerProdImpl<1, L, R>  {
  inline static auto apply(const L& l, const R& r) {
    return std::get<0>(l) * std::get<0>(r);
    // return l[0] * r[0];
  }
};

template <class L, class R>
struct InnerProdImpl<0, L, R>  {
  inline static auto apply(const L& l, const R& r) {
    return 0;
  }
};

template <std::size_t I, std::size_t Last, class Tuple, class Function>
inline typename std::enable_if<I == Last, Function>::type
tuple_for_each_impl(Tuple& t, Function fn) {
  return fn;
}

template <std::size_t I, std::size_t Last, class Tuple, class Function>
inline typename std::enable_if<(I < Last), Function>::type
tuple_for_each_impl(Tuple& t, Function fn) {
  fn(std::get<I>(t));
  return tuple_for_each_impl<I+1, Last>(t, fn);
}

}  // namespace internal

/**
 * @brief access i-th element of variable arguments
 * @code
 * int x=0, y=0, z=0;
 * element_at<0>(x, y, z) = 100;  // x=100
 * @endcode
 * @todo do not use make_tuple
 */
template <std::size_t I, class... Args>
inline auto& element_at(Args&... args) {
  return std::get<I>(std::make_tuple(std::ref(args)...));
}

}  // namespace expression


/**
 * @brief inner product
 *
 * Calculate inner product between tuple-like objects. All operations are
 * expanded at compiling.
 *
 * @code
 * std::vector<int> t {1, 2, 3};
 * std::vector<int> u {2, 3, 4};
 * int n = inner_prod(t, u);  // 20
 * @endcode
 *
 * @pre std::tuple_size and std::get both are available
 */
template <std::size_t N, class L, class R>
inline auto inner_prod(const L& l, const R& r) {
  return expression::internal::InnerProdImpl<N, L, R>::apply(l, r);
}

// Recover me after implemention for tuple
template <class L, class R>
inline auto inner_prod(const L& l, const R& r) {
  static_assert(std::tuple_size<L>::value == std::tuple_size<R>::value,
                "size of tuple must be same." );
  return inner_prod<std::tuple_size<L>::value>(l, r);
}

/**
 * @brief Euclidian norm
 * @tparam N number of elements
 */
template <std::size_t N, class T>
auto euclidean_norm(const T& x) {
  return std::sqrt(inner_prod<N>(x, x));
}

/**
 * @brief use std::tuple_size as number of elements
 */
template <class T>
auto euclidean_norm(const T& x) {
  return std::sqrt(inner_prod<std::tuple_size<T>::value>(x, x));
}

/**
 * @brief call a function for each element of tuple-like object
 *
 * @code
 * auto t = make_tuple(1, 2, 3);
 * tuple_for_each(t, [](int& n){n++;});  // t = (2, 3, 4)
 *
 * int sum = 0;
 * tuple_for_each(t, [&](int n){sum+=n;} // sum = 6
 * @endcode
 */
template <class Tuple, class Function>
Function tuple_for_each(Tuple& t, Function fn) {
  expression::internal::
  tuple_for_each_impl<0, std::tuple_size<Tuple>::value>(t, fn);
  return std::move(fn);
}

}  // namespace particles


namespace std {
template <size_t I, class L, class Op, class R>
inline auto get(const particles::expression::Exp<L, Op, R>& e) {
  return Op::apply(get<I>(e.l), get<I>(e.r));
}
}  // namespace std
