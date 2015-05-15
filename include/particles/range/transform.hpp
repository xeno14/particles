/**
 * @file transform.hpp
 * @brief Add description here
 */
#pragma once

#include "../expression.hpp"
#include "../util.hpp"

#include <iterator>
#include <functional>
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

}  // namespace range

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

}  // namespace particles
