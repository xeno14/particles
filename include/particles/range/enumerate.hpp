/**
 * @file enumerate.hpp
 * @brief Add description here
 */
#pragma once

#include "../expression.hpp"

#include <iterator>
#include <utility>

namespace particles {
namespace range {

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

}  // namespace range

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

}  // namespace particles
