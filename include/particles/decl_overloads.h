/**
 * @file decl_overloads.h
 *
 * @brief Declarations of overloaded stl functions
 */

#pragma once

#include <type_traits>

// particles classes
namespace particles {
template <class T, std::size_t N> 
class Vec;

namespace expression {
template <class L, class Op, class R>
struct Exp;
}  // namespace expression
}  // namespace particles


namespace std {
using namespace particles;

template <typename T> class tuple_size;

// for Particle
template <size_t I, class T, size_t N>
inline T& get(Vec<T, N>& v) noexcept;

template <size_t I, class T, size_t N>
const inline T& get(const Vec<T, N>& v) noexcept;

template <class T, size_t N>
struct tuple_size<Vec<T, N>>
: public integral_constant<std::size_t, N> {};

// for Exp
template <size_t I, class L, class Op, class R>
inline auto get(const expression::Exp<L, Op, R>& e);

}  // namespace std
