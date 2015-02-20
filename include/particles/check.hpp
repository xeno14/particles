/**
 * @file check.hpp
 *
 */

#pragma once

#include <functional>
#include <tuple>
#include <type_traits>


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
    std::get<0>(std::ref(*t).get()),
    std::get<0>(std::cref(*t).get()),
    // TODO move
    // std::get<0>(std::move(*t)),
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

}  // namespace check
}  // namespace particles
