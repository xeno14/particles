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
#include "io.hpp"

#include <functional>
#include <iterator>
#include <memory>
#include <utility>

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

template <class T>
struct Type {
  typedef T type;
};
template <class T>
constexpr Type<T> ref_to_type(T&);
template <class T>
constexpr Type<T> ref_to_type(const T&);

}  // namespace internal


/**
* @brief zip iterator
*
* Iterators are zipped in a tuple.
* @todo use template to specify type of iterator (ref? const?)
*/
template <class Tuple, class Ref, class... Iterator>
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
      return internal::ref_tuple<Ref>(zipped_);
    }
  private:
    Tuple zipped_;
};

/**
 * @brief range for zipped iterators, implementation of zip
 * @todo const iterator
 */
template <class... Range>
class ZipRange {
  typedef std::tuple<typename Range::iterator...> iterator_tuple_type;

  public:
   typedef ZipIterator<iterator_tuple_type, internal::ref, Range...> iterator;
   typedef ZipIterator<iterator_tuple_type, internal::cref, Range...>
       const_iterator;

   ZipRange(Range&... ranges)
       : begins_(std::begin(ranges)...), ends_(std::end(ranges)...) {}

   auto begin() { return iterator(begins_); }
   auto end() { return iterator(ends_); }
   auto cbegin() { return const_iterator(begins_); }
   auto cend() { return const_iterator(ends_); }

  private:
   iterator_tuple_type begins_;
   iterator_tuple_type ends_;
};


template <class T=int>
class XRange {

  public:
    class iterator {
      public:
        iterator(T i) : count_(i) {}
        iterator(const iterator& it) : count_(it.count_) {}
        iterator& operator++() {
          ++count_;
          return *this;
        }
        iterator operator++(int) {
          iterator old(*this);
          operator++();
          return old;
        }
        iterator& operator=(const iterator& it) {
          count_ = it.count_;
          return *this;
        }
        bool operator==(const iterator& it) const {
          return count_ == it.count_;
        }
        bool operator!=(const iterator& it) const {
          return count_ != it.count_;
        }
        T& operator*() { res_ = count_; return std::ref(res_); }
      private:
        T count_;  // changed only by operator++
        T res_;
    };
    
    XRange(T first, T last) : first_(first), last_(last) {}
    auto begin() { return iterator(first_); }
    auto end() { return iterator(last_); }

  private:
    T first_, last_;
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
 * Comparision (operator==, operator!=) is defined between ConvertIterator with
 * other converter and Iterator.
 *
 * Use this through convert_iterator.
 *
 * @see convert_iterator
 * @brief convert iterator
 * @tparam Iterator iterator
 * @tparam Converter rule to convert iterator
 */
template <class Iterator, class Converter>
class ConvertIterator {
 public:
  typedef Converter converter_type;
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
  auto operator-(const ConvertIterator& it) const { return it_ - it.it_; }

 private:
  Iterator it_;
  Converter converter_;
};


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
  typedef typename decltype(internal::ref_to_type(*first))::type value_type;
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
 * auto ave = range::average(u.begin(), u.end());   // 3
 * @endcode
 */
template<class InputIterator>
inline auto average(InputIterator first, InputIterator last) {
  typedef typename decltype(internal::ref_to_type(*first))::type value_type;
  std::size_t num = 0;
  auto s = value_type();
  auto it = first;
  while (it != last) {
    s += *it;
    ++it;
    num++;
  }
  s /= double(num);
  return s;
  /** @todo result s / double(num); why it does not work? */
}

/**
 * @brief call push_back instead of assigning.
 *
 * This class has a reference to a list such as std::vector. When assigning to
 * the value this iterator is pointing, push_back is called.
 */
template <class L>
class PushBackIterator
    : public std::iterator<std::output_iterator_tag, typename L::value_type> {
 private:
  /**
   * @brief Assigning operator is used for push_back
   */
  class PushBackImpl {
   public:
    PushBackImpl(L& l) : l_(l) {}
    template <class T>
    void operator=(const T& x) { this->l_.push_back(x); }

   private:
    L& l_;
  };

 public:
  PushBackIterator(L& l) : l_(l) {}
  PushBackIterator(const PushBackIterator& it) : l_(it.l_) {}
  PushBackIterator& operator=(const PushBackIterator& it) {
    l_ = it.l_;
    return *this;
  }
  PushBackIterator& operator++() { return *this; }
  PushBackIterator& operator++(int) { return *this; }
  PushBackImpl operator*() { return PushBackImpl(l_); }

 private:
  L& l_;
};

}  // namespace range

/**
 * @pre first <= last
 */
template <class T=int>
inline auto xrange(T first, T last) {
  return range::XRange<T>(first, last);
}

/**
 * @brief python-line xrange
 * @todo step
 *
 * @code
 * int sum=0;
 * for (auto n : range::xrange(1, 10)) {  // n=1,2,...,9
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
 * @brief zip iteration
 * @pre 
 *   - all ranges must have same size, otherwise undefined behavior
 *   - range has type iterator (range::iterator)
 *
 * @code
 * std::vector<int> u {1,2,3}, v {4,5,6};
 *
 * for (auto z : range::zip(u, v)) {
 *   auto& x = std::get<0>(z);  // element of u
 *   auto& y = std::get<1>(z);  // element of v
 *   x *= -1;
 *   y *= -1;
 * }
 * // u = {-1,-2,-3}, v = {-4,-5,-6}
 * @endcode
 * @todo const range& (type of iterator should be specified here)
 */
template <class... Range>
inline auto zip(Range&... ranges) {
  return range::ZipRange<Range...>(ranges...);
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
 * @brief wrap ConvertIterator to expect type inference
 * @tparam Iterator iterator
 * @tparam Converter rule to convert iterator
 * @param it iterator to hold
 * @param f converter
 *
 * @code
 * vector<pair<int,int>> v {{1,2}, {3,4}, {5,6}, {7,8}};
 *
 * // Copy. If you want to get reference, use std::ref as return value of
 * // the converter. 
 * auto it convert_iterator(v.begin(), [](pair<int,int>& u){return u.first;});
 * // 1, 3, 5, 7
 * while (it != v.end()) 
 *   cout << *(it++) << endl;
 * }
 * @endcode
 */
template <class Iterator, class Converter>
auto convert_iterator(Iterator it, Converter f) {
  return range::ConvertIterator<Iterator, Converter>(it, f);
}

/**
 * @brief create PushBackIterator
 *
 * @code
 * std::vector<int> v {1, 2, 3, 4};
 * std::vector<int> res {0};
 * // res: {0, 1, 2, 3, 4}
 * std::copy(v.begin(), v.end(), range::push_back_iterator(res));
 * @endcode
 * 
 * @pre type L has member function push_back and member type value_type
 * @tparam L list type (e.g. std::vector)
 * @param l list
 */
template <class L>
auto push_back_iterator(L& l) { return range::PushBackIterator<L>(l); }

}  // namespace particles

using namespace particles;
OVERLOAD_STD_BEGIN_AND_END(class... Range,
                           range::ZipRange<Range...>);
OVERLOAD_STD_BEGIN_AND_END(class T, range::XRange<T>);
