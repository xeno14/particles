/**
 * @file xrange.hpp
 * @brief Add description here
 */
#pragma once

#include "../expression.hpp"

#include <iterator>
#include <functional>
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

}  // namespace range

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

}  // namespace particles

using namespace particles;
OVERLOAD_STD_BEGIN_AND_END(class T, range::XRange<T>);
