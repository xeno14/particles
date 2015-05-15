/**
 * @file join.hpp
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
