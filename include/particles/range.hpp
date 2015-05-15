/**
 * @file range.hpp
 *
 * @todo check iterator concept
 * @todo inherit std::iterator 
 * @todo use macro to define range for a ceartain iterator
 */

#pragma once

#include "check.hpp"
#include "expression.hpp"
#include "util.hpp"
#include "io.hpp"
#include "range/zip.hpp"
#include "range/enumerate.hpp"
#include "range/xrange.hpp"

#include <functional>
#include <iterator>
#include <memory>
#include <utility>

namespace particles {
namespace range {

/**
 * @brief wrap a iteretor and operator* calls a function
 *
 * Comparision (operator==, operator!=) is defined between TransformIterator with
 * other converter and Iterator.
 *
 * Use this through transform_iterator.
 *  
 * TODO rename into transform iterator
 *
 * @see transform_iterator
 * @tparam Iterator iterator
 * @tparam UnaryOperation aaa;
 */
template <class Iterator, class UnaryOperation,
          class ValueType = expression::return_type<
              UnaryOperation, typename Iterator::value_type>>
class TransformIterator
    : public std::iterator<std::input_iterator_tag, ValueType> {
 public:
  TransformIterator(Iterator it, UnaryOperation op) : it_(it), op_(op) {}
  TransformIterator(const TransformIterator& cit)
      : it_(cit.it_), op_(cit.op_) {}
  TransformIterator& operator++() {
    ++it_;
    return *this;
  }
  TransformIterator operator++(int) {
    auto res = *this;
    operator++();
    return res;
  }

  auto operator*() { return op_(*it_); }

  bool operator==(const TransformIterator& cit) const { return it_ == cit.it_; }
  bool operator!=(const TransformIterator& cit) const { return it_ != cit.it_; }

  template <class F>
  bool operator==(const TransformIterator<Iterator, F>& cit) const {
    return it_ == cit.it();
  }
  template <class F>
  bool operator!=(const TransformIterator<Iterator, F>& cit) const {
    return it_ != cit.it();
  }
  bool operator==(const Iterator& it) const { return it_ == it; }
  bool operator!=(const Iterator& it) const { return it_ != it; }

  template <class F>
  auto operator-(const TransformIterator<Iterator, F>& it) const {
    return std::distance(it_, it.it_);
  }
  const Iterator& it() const { return it_; }

 private:
  Iterator it_;
  UnaryOperation op_;
};

/**
 * @tparam Iterator actual_iterator
 *
 * @pre Iterator should be forward_iterator
 * @todo change iterator_tag up to Iterator's tag
 */
template <class Iterator, std::size_t N>
class JoinedIterator
  : public std::iterator<
      std::forward_iterator_tag,
      typename std::iterator_traits<Iterator>::value_type> {
 public:
  JoinedIterator(std::initializer_list<Iterator> firsts,
                 std::initializer_list<Iterator> lasts,
                 std::size_t pos = 0) : pos_(pos) {
    auto it1 = std::begin(firsts);
    auto it2 = std::begin(lasts);
    std::size_t i = 0;
    while (it1 != std::end(firsts) && it2 != std::end(lasts)) {
      iterators_[i].first  = *it1;
      iterators_[i].second = *it2;
      ++it1; ++it2; ++i;
    }
  }

  JoinedIterator(const JoinedIterator& it)
    : pos_(it.pos_), iterators_(it.iterators_){}

  JoinedIterator& operator++() {
    ++iterators_[pos_].first;
    if (iterators_[pos_].first == iterators_[pos_].second) ++pos_;
    return *this;
  }

  JoinedIterator operator++(int) {
    JoinedIterator<Iterator, N> res = *this;
    operator++();
    return res;
  }

  auto& operator*() { return *iterators_[pos_].first; }

  bool operator==(const JoinedIterator& it) const {
    return pos_ == it.pos_ && iterators_ == it.iterators_;
  }
  bool operator!=(const JoinedIterator& it) const { return !(*this == it); }

 private:
  std::size_t pos_;
  std::array<std::pair<Iterator, Iterator>, N> iterators_;
};

/**
 * @tparam Range... type of ranges
 * @pre Range... have same iterator
 */
template <class... Range>
class JoinedRange {
  static_assert(check::has_same_iterator<Range...>{},
                "Requires same iterator type");
  typedef typename expression::PickHead<
            typename Range::iterator...>::type actual_iterator;
 public:
  typedef JoinedIterator<actual_iterator, sizeof...(Range)> iterator;

  JoinedRange(Range&... ranges)
    : begin_({std::begin(ranges)...}, {std::end(ranges)...}, 0),
      end_({std::end(ranges)...}, {std::end(ranges)...}, sizeof...(Range)) {}

  iterator begin() const { return begin_; }
  iterator end() const { return end_; }

 private:
  iterator begin_, end_;
};

}  // namespace range


/**
 * @brief sum over iterators
 *
 * @code
 * std::vector<int> u {1, 2, 3};
 * auto s = range::sum(u.begin(), u.end());  // 6
 * @endcode
 */
template <class InputIterator>
inline auto sum(InputIterator first, InputIterator last) {
  typedef typename std::remove_reference<decltype(*first)>::type value_type;
  // get value type
  // avoid uninitialized warning
  auto res = value_type();
  auto it = first;
  while (it != last) {
    res += *it;
    ++it;
  }
  return res;
}

/**
 * @todo return double when calculating average over list of int
 *
 * @brief average
 *
 * @code
 * vector<double> u {1, 2, 3};
 * average(u.begin(), u.end());    // 3
 *
 * // for integral types, result is in float.
 * vector<int> v {1, 2};
 * average(v.begin(), v.begin());  // 1.5
 * @endcode
 */
template<class InputIterator>
inline auto average(InputIterator first, InputIterator last) {
  typedef typename std::remove_reference<decltype(*first)>::type value_type;
  typedef typename util::type_cond<
                      std::is_integral<value_type>::value,
                      double, value_type>::type result_type;
  std::size_t num = 0;
  auto s = result_type();
  auto it = first;
  while (it != last) {
    s += *it;
    ++it;
    num++;
  }
  s /= double(num);
  return s;
  /** @todo return s / double(num); why it does not work for Vec? */
}

/**
 * @brief python-line xrange
 *
 * @code
 * int sum=0;
 * for (auto n : xrange(1, 10)) {  // 1, 2,..., 9
 *   sum+=n;
 * }
 * cout << n << endl;   // 45
 * @endcode
 */
template <class T=std::size_t>
inline auto xrange(T last) {
  return xrange<T>(0, last);
}

/**
 * @brief wrap TransformIterator to expect type inference
 * @tparam Iterator iterator
 * @tparam UnaryOperation rule to convert iterator
 * @param it iterator to hold
 * @param f converter
 *
 * @code
 * vector<pair<int,int>> v {{1,2}, {3,4}, {5,6}, {7,8}};
 *
 * // Copy. If you want to get reference, use std::ref as return value of
 * // the converter. 
 * auto it transform_iterator(v.begin(), [](pair<int,int>& u){return u.first;});
 * // 1, 3, 5, 7
 * while (it != v.end()) 
 *   cout << *(it++) << endl;
 * }
 * @endcode
 */
template <class Iterator, class UnaryOperation>
auto transform_iterator(Iterator it, UnaryOperation f) {
  return range::TransformIterator<Iterator, UnaryOperation>(it, f);
}

/**
 * @brief pair of transform_iterator
 * @code
 * vector<int> v{1, 2, 3};
 * auto p = transform_iterator(v.begin(), v.end(), [](int n){return 2*n});
 * std::copy(p.first, p.second, result);
 * @endcode
 */
template <class Iterator, class UnaryOperation>
auto transform_iterator(Iterator first, Iterator last, UnaryOperation f) {
  return std::make_pair(transform_iterator(first, f),
                        transform_iterator(last, f));
}

/**
 * @brief Make joined range
 * @pre All containers have same iterator type
 *
 * For everytime the iterator is incremented, checks if it reaches the end or
 * not. Therefore, iteration has an inevitable overhead.
 *
 * @code
 * vector<int> u {1, 2, 3};
 * vector<int> v {4, 5, 6};
 * vector<int> result;
 *
 * // result: 1 2 3 4 5 6
 * for (int n : make_joined(u, v)) {
 *   result.push_back(n);
 * }
 * @endcode
 */
template <class... Range>
auto make_joined(Range&... ranges) {
  return range::JoinedRange<Range...>(ranges...);
}

}  // namespace particles

using namespace particles;
OVERLOAD_STD_BEGIN_AND_END(class... Range, range::JoinedRange<Range...>);
