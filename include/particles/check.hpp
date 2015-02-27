/**
 * @file check.hpp
 *
 */

#pragma once

#include <functional>
#include <tuple>
#include <type_traits>

#include "decl_overloads.h"

namespace particles {
namespace check {
namespace internal {

template <bool Cond>
struct Conditional {
  typedef typename std::conditional<Cond, std::true_type, std::false_type>::type
      type;
};

struct IsStdGetOverloadedImpl {
  template <class T>
  static auto check(T* t) -> decltype(
    std::get<0>(*t),
    // TODO check for ref(*t).get(), cref(*t).get(), move(*t)
    std::true_type());

  template <class T>
  static auto check(...) -> std::false_type;
};

struct HasTupleSizeImpl {
  template <class T>
  static auto check(T* t) -> decltype(
    std::tuple_size<T>::value,
    std::true_type());

  template <class T>
  static auto check(...) -> std::false_type;
};

struct HasAccessOperatorImpl {
  template <class T>
  static auto check(T* t) -> decltype((*t)[std::size_t(0)], std::true_type());

  template <class T>
  static auto check(...) -> std::false_type;
};

template <template <class, class> class F, class T,  class Head, class... Tail>
struct HasSameImpl {
  static const bool value
      = F<T, Head>::value && HasSameImpl<F, T, Tail...>::value;
};

template <template <class, class> class F, class T, class U>
struct HasSameImpl<F, T, U> {
  static const bool value = F<T, U>::value;
};

/**
 * @tparam F binary meta function that has boolean value 
 */
template <template <class, class> class F, class Head, class... Tail>
struct HasSame
  : public std::conditional<
      HasSameImpl<F, Head, Head, Tail...>::value,
      std::true_type,
      std::false_type>::type {};

template <class T, class U>
struct HasSameValueType {
  static const bool value =
    std::is_same<
      typename T::value_type,
      typename U::value_type
    >::value;
};

}  // namespace internal

/**
 * @brief Check is std::get is overloaded for a class
 */
template <class T>
struct is_get_overloaded
    : public decltype(internal::IsStdGetOverloadedImpl::check<T>(nullptr)) {};

/**
 * @brief Check is std::tuple_size is specialized for a class
 */
template <class T>
struct has_tuple_size
    : public decltype(internal::HasTupleSizeImpl::check<T>(nullptr)) {};

/**
 * @brief Check if a class is like std::tuple
 *
 * Same as std::true_type if the given class satisfies:
 *   - std::get is overloaded
 *   - std::tuple_size is specialized
 * @code
 * static_assert(!is_tuple_like<int>{}, "int");                // false_type
 * static_assert( is_tuple_like<std::tuple<int>>{}, "tuple");  // true_type
 * @code
 */
template <class T>
struct is_tuple_like
    : public internal::Conditional<
          is_get_overloaded<T>{} && has_tuple_size<T>{}>::type {};

/**
 * @brief Check if a class has operator[]
 */
template <class T>
struct has_access_operator
    : public decltype(internal::HasAccessOperatorImpl::check<T>(nullptr)) {};

/**
 * @brief Check if all arguments have same value_type
 */
template <class... Args>
struct has_same_value_type
  : public internal::HasSame<
    internal::HasSameValueType,
    Args...
  > {
  static_assert(sizeof...(Args)>=2, "Requires at least 2 template arguments");  
};

template <class... Args>
struct has_same_iterator
  : public internal::HasSame<
      std::is_same,
      typename Args::iterator...
    > {
  static_assert(sizeof...(Args)>=2, "Requires at least 2 template arguments");  
};

}  // namespace check
}  // namespace particles
