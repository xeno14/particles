#pragma once

#include <tuple>
#include <cstdlib>
#include <iostream>

namespace particles {

namespace expression {

template <std::size_t N, class L, class R>
struct Assign {
  static void apply(L& l, const R& r) {
    std::get<N>(l) = std::get<N>(r);
    Assign<N - 1, L, R>(l, r);
  }
};

template <class L, class R>
struct Assign<0, L, R> {
  static void apply(L& l, const R& r) { std::get<0>(l) = std::get<0>(r); }
};

template <class L, class Op, class R>
struct Expression {
  const L& l;
  const R& r;

  Expression(const L& l, const R& r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }
};

template <class T>
struct Plus {
  static T apply(T l, T r) { return l + r; }
};

template <class T>
struct Minus {
  static T apply(T l, T r) { return l - r; }
};

template <class T>
struct Multiple {
  static T apply(T l, T r) { return l * r; }
};

}  // namespace expression

using expression::Expression;
using expression::Plus;
using expression::Minus;
using expression::Multiple;

template <class L, class R>
inline Expression<L, Plus<typename R::value_type>, R>
operator+(const L& l, const R& r) {
  typedef typename R::value_type value_type;
  return Expression<L, Plus<value_type>, R>(l, r);
}

template <class L, class R>
inline Expression<L, Minus<typename R::value_type>, R>
operator-(const L& l, const R& r) {
  typedef typename R::value_type value_type;
  return Expression<L, Minus<value_type>, R>(l, r);
}

}  // namespace particles
