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

#include <functional>
#include <iterator>
#include <memory>
#include <utility>

namespace particles {
namespace range {


/**
 * @brief suite last to make sure counting up by step does not skip over
 *
 * @tparam T integral_type
 */
template <class T>
T suite_last(T first, T last, T step) {
  if ((last - first) % step != 0) {
    return last + step - (last - first) % step;
  }
  return last;
}

template <class T=int>
class XRange {
  public:
    class iterator : public std::iterator<std::input_iterator_tag, T> {
      public:
        iterator(T i, T step) : count_(i), step_(step) {}
        iterator(const iterator& it) : count_(it.count_), step_(it.step_) {}
        iterator& operator++() {
          count_ += step_;
          return *this;
        }
        iterator operator++(int) {
          iterator old(*this);
          operator++();
          return old;
        }
        iterator& operator=(const iterator& it) {
          count_ = it.count_; step_ = it.step_;
          return *this;
        }
        bool operator==(const iterator& it) const {
          return count_ == it.count_;
        }
        bool operator!=(const iterator& it) const {
          return count_ != it.count_;
        }
        T operator*() const { return count_; }
      private:
        T count_, step_;
    };
    
    XRange(T first, T last, T step=1) :
        first_(first), last_(suite_last(first, last, step)), step_(step) {}
    auto begin() { return iterator(first_, step_); }
    auto end() { return iterator(last_, step_); }

  private:
    T first_, last_, step_;
};


template <class Range>
class EnumerateRange {
  typedef std::pair<std::size_t, typename Range::iterator> iterator_pair_type;
  public:
   class iterator {
     public:
     iterator(const iterator& it) : iterator_pair_(it.iterator_pair_) {}
     iterator(const iterator_pair_type& p) : iterator_pair_(p) {}
     iterator operator++() {
       expression::pre_increment<2>(iterator_pair_);
       return *this;
     }
     iterator operator++(int) {
       iterator old(*this);
       operator++();
       return old;
     }
     iterator& operator=(const iterator& it) {
       iterator_pair_ = it.iterator_pair_;
       return *this;
     }
     bool operator==(const iterator& rhs) const {
       return iterator_pair_ == rhs.iterator_pair_;
     }
     bool operator!=(const iterator& rhs) const {
       return iterator_pair_ != rhs.iterator_pair_;
     }
     auto operator*() {
       return std::make_pair(std::get<0>(iterator_pair_),
                              std::ref(*(std::get<1>(iterator_pair_))));
     }
    private:
     iterator_pair_type iterator_pair_;
   };

   EnumerateRange(Range& range, std::size_t start = 0)
       : begin_(start, std::begin(range)),
         end_(start +
                  static_cast<std::size_t>(std::end(range) - std::begin(range)),
              std::end(range)) {}
   EnumerateRange(const EnumerateRange& enum_range)
       : begin_(enum_range.begin_), end_(enum_range.end_) {}
   iterator begin() { return iterator(begin_); }
   iterator end() { return iterator(end_); }

  private:
    iterator_pair_type begin_;
    iterator_pair_type end_;
};

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
 * @brief python-like xrange
 *
 * @code
 * // 0, 2, 4
 * for (auto n : xrange(0, 6, 2)) {
 *   ...
 * }
 * // 0, 2, 4
 * for (auto n : xrange(0, 5, 2)) {
 *   ...
 * }
 * @pre first <= last
 */
template <class T=int>
inline auto xrange(T first, T last, T step=1) {
  return range::XRange<T>(first, last, step);
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
 * @brief python-like enumerate
 *
 * @code
 * vector<int> v = {1,2,3};
 * for (auto e : range::enumerate(v)) {
 *   e.first;        // 0, 1, 2
 *   e.second += 2;  // v[0]+=2, v[1]+=2, v[2]+=2
 * }
 * @endcode
 */
template <class Range>
inline auto enumerate(Range& range, std::size_t start=0) {
  return range::EnumerateRange<Range>(range, start);
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
OVERLOAD_STD_BEGIN_AND_END(class T, range::XRange<T>);
OVERLOAD_STD_BEGIN_AND_END(class... Range, range::JoinedRange<Range...>);
