/**
 * @file zip.hpp
 * @brief Add description here
 */
#pragma once

#include "../range/common.hpp"
#include "../check.hpp"
#include "../expression.hpp"
#include "../util.hpp"
#include "../io.hpp"

#include <iterator>

namespace particles {
namespace range {

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

}  // namespace range


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

}  // namespace particles
