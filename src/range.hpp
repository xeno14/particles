/**
 * @file range.hpp
 *
 * @todo check iterator concept
 * @todo zip
 * @todo enumerate
 */

#pragma once

#include "expression.hpp"

#include <functional>

namespace particles {
namespace range {
namespace internal {

/**
 *
 * @tparam loop_flag
 * @tparam Ts
 * @tparam Us reference
 */
template <bool loop_flag>
struct RefTupleImpl {
  template <class... Iterator, class... Refs>
  static auto make_tuple(std::tuple<Iterator...>& t, Refs... refs) {
    constexpr bool next_flag = sizeof...(Iterator) - 1 > sizeof...(Refs);
    constexpr std::size_t I = sizeof...(Refs);
    return RefTupleImpl<next_flag>::make_tuple(t, refs...,
                                               std::ref(*std::get<I>(t)));
  }
};
template <>
struct RefTupleImpl<false> {
  template <class... Iterator, class... Refs>
  static auto make_tuple(std::tuple<Iterator...>& t, Refs... refs) {
    return std::make_tuple(refs...);
  }
};
template <class... Iterator>
inline auto ref_tuple(std::tuple<Iterator...>& t) {
  return RefTupleImpl<true>::make_tuple(t);
}

}  // namespace internal

template <class Iterator, class Converter>
auto convert_iterator(Iterator it, Converter f);

/**
 * Comparision (operator==, operator!=) is defined between ConvertIterator with
 * other converter and Iterator.
 *
 * Use this through convert_iterator.
 * @code
 * vector<vector<int>> v {{1,2}, {3,4}, {5,6}, {7,8}};
 * auto it convert_iterator(v.begin(), [](vector<int>& u){return u[0];});
 * // 1, 3, 5, 7
 * while (it != v.end()) 
 *   cout << *(it++) << endl;
 * }
 * @endcode
 *
 * @see convert_iterator
 * @brief convert iterator
 * @tparam Iterator iterator
 * @tparam Converter rule to convert iterator
 */
template <class Iterator, class Converter>
class ConvertIterator {
 public:
  ConvertIterator(Iterator it, Converter f) : it_(it), converter_(f) {}

  auto operator*() { return converter_(*it_); }

  auto operator++() { ++it_; return *this; }
  auto operator++(int) { auto res = *this; ++it_; return res; }

  ConvertIterator& operator=(const ConvertIterator<Iterator, Converter>& cit) {
    it_ = cit.it_;
    return *this;
  }
  template <class F>
  bool operator==(const ConvertIterator<Iterator, F>& cit) const {
    return it_ == cit.it_;
  }
  template <class F>
  bool operator!=(const ConvertIterator<Iterator, F>& cit) const {
    return it_ != cit.it_;
  }
  bool operator==(const Iterator& it) const { return it_ == it; }
  bool operator!=(const Iterator& it) const { return it_ != it; }

 private:
  Iterator it_;
  Converter& converter_;
};

/**
 * @brief wrap ConvertIterator to expect type inference
 * @tparam Iterator iterator
 * @tparam Converter rule to convert iterator
 * @param it iterator to hold
 * @param f converter
 */
template <class Iterator, class Converter>
auto convert_iterator(Iterator it, Converter f) {
  return ConvertIterator<Iterator, Converter>(it, f);
}



}  // namespace range
}  // namespace particles
