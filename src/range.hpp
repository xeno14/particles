/**
 * @file range.hpp
 *
 * @todo check iterator concept
 * @todo inherit std::iterator 
 * @todo use macro to define range for a ceartain iterator
 */

#pragma once

#include "expression.hpp"
#include "util.hpp"

/** @todo remove me */
#include <iostream>

#include <functional>
#include <iterator>
#include <memory>
#include <utility>

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
* @brief zip iterator
*
* Iterators are zipped in a tuple.
* @todo use template to specify type of iterator (ref? const?)
*/
template <class Tuple, class... Iterator>
class ZipIterator {
  public:
    ZipIterator(const ZipIterator& it) : zipped_(it.zipped_) {}
    ZipIterator(const Tuple& zipped) : zipped_(zipped) {}
    ZipIterator operator++() {
      constexpr std::size_t sz = std::tuple_size<Tuple>::value;
      expression::pre_increment<sz>(zipped_);
      return ZipIterator(*this);
    }
    ZipIterator operator++(int) {
      ZipIterator old(*this);
      operator++();
      return old;
    }
    ZipIterator& operator=(const ZipIterator& it) {
      zipped_ = it.zipped_;
      return *this;
    }
    bool operator==(const ZipIterator& it) const {
      return zipped_ == it.zipped_;
    }
    bool operator!=(const ZipIterator& it) const {
      return zipped_ != it.zipped_;
    }
    auto operator*() {
      return internal::ref_tuple(zipped_);
    }
  private:
    Tuple zipped_;
};

/**
 * @brief range for zipped iterators, implementation of zip
 * @todo const iterator
 */
template <class... Range>
class ZipContainer {
  typedef std::tuple<typename Range::iterator...> iterator_tuple_type;

  public:
   typedef ZipIterator<iterator_tuple_type, Range...> iterator;

   ZipContainer(Range&... ranges)
       : begins_(std::begin(ranges)...), ends_(std::end(ranges)...) {}

   auto begin() { return iterator(begins_); }
   auto end() { return iterator(ends_); }

  private:
   iterator_tuple_type begins_;
   iterator_tuple_type ends_;
};

/**
 * @brief zip iteration
 * @todo const range&
 * @pre 
 *   - all ranges must have same size, otherwise undefined behavior
 *   - range has type iterator (range::iterator)
 * @code
 * std::vector<int> u {1,2,3}, v {4,5,6};
 *
 * for (auto z : range::zip(u, v)) {
 *   auto x = std::get<0>(z);  // element of u
 *   auto y = std::get<1>(z);  // element of v
 *   x *= -1;
 *   y *= -1;
 * }
 * // u = {-1,-2,-3}, v = {-4,-5,-6}
 * @endcode
 */
template <class... Range>
inline auto zip(Range&... ranges) {
  return ZipContainer<Range...>(ranges...);
}


template <class Range>
class EnumerateRange {
  typedef std::tuple<std::size_t, typename Range::iterator> iterator_pair_type;
  public:
   class iterator {
     public:
     iterator(const iterator& it) : iterator_pair_(it.iterator_pair_) {}
     iterator(const iterator_pair_type& p) : iterator_pair_(p) {}
     iterator& operator=(const iterator& it) {
       iterator_pair_ = it.iterator_pair_;
       return *this;
     }
     iterator& operator++() {
       expression::pre_increment<2>(iterator_pair_);
       return *this;
     }
     iterator operator++(int) {
       auto old = iterator_pair_;
       ++(*this);
       return iterator(old);
     }
     bool operator==(const iterator& rhs) const {
       return iterator_pair_ == rhs.iterator_pair_;
     }
     bool operator!=(const iterator& rhs) const {
       return iterator_pair_ != rhs.iterator_pair_;
     }
     auto operator*() {
       return std::make_tuple(std::get<0>(iterator_pair_),
                              std::ref(*(std::get<1>(iterator_pair_))));
     }
    private:
     iterator_pair_type iterator_pair_;
   };

   EnumerateRange(Range& range, std::size_t start = 0)
       : begin_(std::make_pair(start, std::begin(range))),
         end_(std::make_pair(start + static_cast<std::size_t>(
                                         std::end(range) - std::begin(range)),
                             std::end(range))) {}
   EnumerateRange(const EnumerateRange& enum_range)
       : begin_(enum_range.begin_), end_(enum_range.end_) {}
   auto begin() { return begin_; }
   auto end() { return end_; }

  private:
    iterator begin_;
    iterator end_;
};

/**
 * @todo something is wrong when using with for statement
 */
template <class Range>
inline auto enumerate(Range range, std::size_t start=0) {
  return EnumerateRange<Range>(range, start);
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

/**
 * @brief enumerate macro
 * @todo remove this after fixing enumerate
 *
 * Usage:
 *
 * @code
 * std::vector<int> v {1, 2, 3};
 * ENUMERATE_BEGIN(i, x, v) {
 *   x += i;
 * } ENUMERATE_END;
 * // v = {1, 3, 5}
 * @endcode
 */
#define ENUMERATE_BEGIN(I, ELEM, RANGE) { \
  particles::range::EnumerateRange<decltype(RANGE)> enum_range(RANGE, 0); \
  auto it = enum_range.begin(); \
  while(it != enum_range.end()) { \
    std::size_t I = std::get<0>(*it); \
    auto& ELEM = std::get<1>(*it);

#define ENUMERATE_END \
    ++it; \
  } \
}
