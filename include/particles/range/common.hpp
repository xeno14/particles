/**
 * @file common.hpp
 * @brief Add description here
 */
#pragma once

#include <functional>


namespace particles {
namespace range {
namespace internal {

struct ref {
  template <class T>
  static auto get(T& elem) noexcept { return std::ref(elem); }
};

struct cref {
  template <class T>
  static auto get(T& elem) noexcept { return std::ref(elem); }
};

/**
 * @brief implementation of ref_tuple
 * @tparam loop_flag
 * @tparam Ts
 * @tparam Us reference
 */
template <class Ref, bool loop_flag>
struct RefTupleImpl {
  template <class... Iterator, class... Refs>
  static auto make_tuple(std::tuple<Iterator...>& t, Refs... refs) {
    constexpr bool next_flag = sizeof...(Iterator) - 1 > sizeof...(Refs);
    constexpr std::size_t I = sizeof...(Refs);
    return RefTupleImpl<Ref, next_flag>::make_tuple(t, refs...,
                                                    Ref::get(*std::get<I>(t)));
  }
};
template <class Ref>
struct RefTupleImpl<Ref, false> {
  template <class... Iterator, class... Refs>
  static auto make_tuple(std::tuple<Iterator...>& t, Refs... refs) {
    return std::make_tuple(refs...);
  }
};

/**
 * @brief creates tuple of references from iterator
 *
 * Example:
 *
 * @code
 * std::vector<int> u {1, 2, 3}, v {4, 5, 6};
 * auto t = range::ref_tuple(std::make_tuple(u.begin(), v.begin()));
 * std::get<0>(t) -= 1;  // u = {0, 2, 3}
 * std::get<1>(t) += 5;  // v = {9, 5, 7}
 * @endcode
 */
template <class Ref, class... Iterator>
inline auto ref_tuple(std::tuple<Iterator...>& t) {
  return RefTupleImpl<Ref, true>::make_tuple(t);
}

}  // namespace internal
}  // namespace range
}  // namespace particles
