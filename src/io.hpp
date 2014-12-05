/**
 * @file io.hpp
 *
 * @brief input and output including global overloading
 */

#include "vec.hpp"
#include "particle.hpp"
#include <iostream>
#include <fstream>
#include <string>

namespace {

constexpr char kNullChar = '\0';
constexpr auto kNullString = "";

}  // anonymous namespace

namespace particles {
namespace io {
namespace internal {


template <std::size_t I>
struct ToOStreamImpl {
  template <class T, class Str>
  static std::ostream& apply(std::ostream& os, const T& t,
                             const Str& delimiter) {
    ToOStreamImpl<I-1>::apply(os, t, delimiter);
    return os << delimiter <<  std::get<I - 1>(t);
  }
};

template <>
struct ToOStreamImpl<1> {
  template <class T, class Str>
  static std::ostream& apply(std::ostream& os, const T& t,
                             const Str& delimiter) {
    return os << std::get<0>(t);
  }
};

}  // namespace internal

/**
 * @brief output to std::ostream
 * @pre std::get<I> is overloaded for T
 * @param o
 * @param t
 * @param delimiter 
 * @param prefix
 * @param suffix
 * @tparam N num of elements
 * @tparam T e.g. std::tuple, particles::Vec
 * @example
 * @code
 *  auto t = std::make_tuple(1,2,3);
 *  //1 2 3
 *  io::output(std::cout, t);
 *  //[1, 2, 3]
 *  io::output(std::cout, t, ", ", "[", "]");
 * @endcode
 */
template <std::size_t N, class T>
std::ostream& output(std::ostream& os, const T& t,
                     const std::string& delimiter = " ",
                     const std::string& prefix = "",
                     const std::string& suffix = "") {
  os << prefix;
  internal::ToOStreamImpl<N>::apply(os, t, delimiter);
  return os << suffix;
}

}  // namespace io
}  // namespace particles

// Global overloading
namespace std {

/**
 * @example
 * @code
 * Vec<int, 3> v {1,2,3};
 * std::cout << v;  // [1, 2, 3]
 * @endcode
 */
template <class T, size_t N>
ostream& operator<<(ostream& os, particles::Vec<T,N>& v) {
  return particles::io::output<N>(os, v, ", ", "[", "]"); 
}

template <class T, size_t N>
ostream& operator<<(ostream& os, particles::Particle<T,N>& p) {
  os << '{';
  particles::io::output<N>(os, p.position(), ", ", "[", "]");
  os << ", ";
  particles::io::output<N>(os, p.velocity(), ", ", "[", "]");
  return os << ", " << p.mass() << '}';
}

/** @todo to_string */

}  // namespace std
