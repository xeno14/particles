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

constexpr auto kNullString = "";

}  // anonymous namespace

namespace particles {
namespace io {
namespace internal {

/**
 * @brief expand output to std::ostream
 * @todo do not have to restrict to use of std::ostream (use template)
 */
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
 * @pre std::get<I> is overloaded for T
 * @param o
 * @param t
 * @param delimiter 
 * @param prefix
 * @param suffix
 * @tparam N num of elements
 * @tparam T e.g. std::tuple, particles::Vec
 *
 * Example:
 * @code
 *  auto t = std::make_tuple(1,2,3);
 *
 *  //1 2 3
 *  io::output(std::cout, t);
 *
 *  //[1, 2, 3]
 *  io::output(std::cout, t, ", ", "[", "]");
 * @endcode
 *
 * @todo overload for vec and particle
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

/**
 * @brief output position and velocity of a particle  to std::ostream
 *
 * Example:
 * @code
 * Particle<int, 2> p({1,2},{3,4});
 *
 * // 1 2 3 4
 * io::output_particle(std::cout, p);
 *
 * // 1,2,3,4
 * io::output_particle(std::cout, p, ",");
 * @endcode
 *
 * @tparam T value type
 * @tparam N dimension
 */
template <class T, std::size_t N>
std::ostream& output_particle(std::ostream& os, const Particle<T, N>& p,
    const std::string& delimiter=" ") {
  internal::ToOStreamImpl<N>::apply(os, p.position(), delimiter);
  os << delimiter;
  internal::ToOStreamImpl<N>::apply(os, p.velocity(), delimiter);
  return os;
}

/**
 * @brief output particles via iterator
 * @tparam Iterator input iterator
 * @param newline output string after each particles
 *
 * @code
 * //1 2 3 4
 * //5 6 7 8
 * io::output_particles(std::cout, v.begin(), v.end());
 *
 * // "1,2,3,4@5,6,7,8@"
 * io::output_particles(std::cout, v.begin(), v.end(), ",", "@");
 * @endcode
 */
template <class Iterator>
std::ostream& output_particles(std::ostream& os, Iterator first, Iterator last,
                               const std::string& delimiter = " ",
                               const std::string& newline = "\n") {
  auto it = first;
  while (it != last) {
    output_particle(os, *it, delimiter);
    os << newline;
    ++it;
  }
  return os;
}

}  // namespace io
}  // namespace particles

// Global overloading
namespace std {

/**
 * @code
 * Vec<int, 3> v {1,2,3};
 * std::cout << v;  // [1, 2, 3]
 * @endcode
 *
 * @brief operator<< for Vec
 *
 * @todo move to vec.hpp
 */
template <class T, size_t N>
ostream& operator<<(ostream& os, particles::Vec<T,N>& v) {
  return particles::io::output<N>(os, v, ", ", "[", "]"); 
}

/**
 * @code
 * Particle<int, 2> p({1,2},{3,4},5);
 * std::cout << p;  // {[1, 2], [3, 4], 5}
 * @endcode
 *
 * @brief operator<< for Particle
 *
 * @todo move to particle.hpp
 */
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
