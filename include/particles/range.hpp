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
#include "range/enumerate.hpp"
#include "range/join.hpp"
#include "range/xrange.hpp"
#include "range/zip.hpp"
#include "range/transform.hpp"

#include <functional>
#include <iterator>
#include <memory>
#include <utility>

namespace particles {

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


}  // namespace particles
