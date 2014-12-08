/**
 * @file range.hpp
 *
 * @todo check iterator concept
 * @todo enumerate
 */

#pragma once

#include "expression.hpp"
#include "util.hpp"

#include <functional>
#include <iterator>

namespace particles {
namespace range {
namespace internal {

/**
 * @brief implementation of ref_tuple
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
template <class... Iterator>
inline auto ref_tuple(std::tuple<Iterator...>& t) {
  return RefTupleImpl<true>::make_tuple(t);
}

}  // namespace internal

/**
 * @brief range for zipped iterators, implementation of zip
 * @todo const iterator
 */
template <class... Range>
class ZipContainer {
  typedef std::tuple<typename Range::iterator...> iterator_tuple_type;

  public:
   ZipContainer(Range&... ranges)
       : begins_(std::begin(ranges)...), ends_(std::end(ranges)...) {}

   /**
    * @brief zip iterator
    *
    * Iterators are zipped in a tuple.
    * @todo use template to specify type of iterator (ref? const?)
    */
   class iterator {
    public:
      iterator(const iterator& it) : iterator_tuple_(it.iterator_tuple_) {}
      iterator(const iterator_tuple_type& it) : iterator_tuple_(it) {}
      iterator operator++() {
        expression::pre_increment<sizeof...(Range)>(iterator_tuple_);
        return *this;
      }
      iterator operator++(int) { 
        auto old = iterator_tuple_;
        ++(*this);
        return iterator(old);
      }
      iterator& operator=(const iterator& it) {
        iterator_tuple_ = it.iterator_tuple_;
        return *this;
      }
      bool operator==(const iterator& it) const {
        return iterator_tuple_ == it.iterator_tuple_;
      }
      bool operator!=(const iterator& it) const {
        return !(*this == it);
      }
      auto operator*() {
        return internal::ref_tuple(iterator_tuple_);
      }

    private:
      iterator_tuple_type iterator_tuple_;
   };

   auto begin() { return iterator(begins_); }
   auto end() { return iterator(ends_); }

  private:
   iterator_tuple_type begins_;
   iterator_tuple_type ends_;
};

/**
 * @brief zip iteration
 * @pre all ranges must have same size, otherwise undefined behavior
 * @code
 * std::vector<int> u {1,2,3}, v {4,5,6};
 * for (auto z : range::zip(u, v)) {
 *   auto x = std::get<0>(z);  // element of u
 *   auto y = std::get<1>(z);  // element of v
 * }
 * @endcode
 */
template <class... Range>
inline auto zip(Range&... ranges) {
  return ZipContainer<Range...>(ranges...);
}

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

using namespace particles;
OVERLOAD_STD_BEGIN_AND_END(class... Range,
                           range::ZipContainer<Range...>);
