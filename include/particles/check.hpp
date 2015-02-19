/**
 * @file check.hpp
 *
 */

#pragma once

#include <tuple>
#include <type_traits>


namespace particles {
namespace check {
namespace internal {

struct IsTupleLikeImpl {
  template <class T>
  static auto check(T* t) -> decltype(
    std::get<0>(*t), std::tuple_size<T>::value, std::true_type());

  template <class T>
  static auto check(...) -> std::false_type;
};

struct HasAccessOperator {
  template <class T>
  static auto check(T* t) -> decltype((*t)[std::size_t(0)], std::true_type());

  template <class T>
  static auto check(...) -> std::false_type;
};

}  // namespace internal


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
    : public decltype(internal::IsTupleLikeImpl::check<T>(nullptr)) {};

/**
 * @brief Check if a class has operator[]
 */
template <class T>
struct has_access_operator
    : public decltype(internal::HasAccessOperator::check<T>(nullptr)) {};

}  // namespace check
}  // namespace particles
