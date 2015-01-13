#pragma once

#include "util.hpp"

#include <cmath>
#include <vector>

#include <iostream>
#include <iterator>

namespace particles {

/**
 * @brief Euclidean distance
 */
template <class InputInterator1, class InputInterator2>
auto euclidean_distance(InputInterator1 first1, InputInterator1 last1,
                        InputInterator2 first2) {
  TYPE_FROM_REF(*first1) sum = 0;
  while (first1 != last1) {
    sum += (*first1 - *first2) * (*first1 - *first2);
    ++first1; ++first2;
  }
  return std::sqrt(sum);
}

/**
 * @brief Simultaneous equations solver using Gauss-Seidel method
 *
 * \f[
 *  a_{ij} = \mathrm{A[i*n + j]}
 * \f]
 *
 * @pre std::begin and std::end are overloaded for InputRange1, InputRange2, OutputRange
 *
 * @return true if num of interations is less than limit, otherwise false
 */
template <class InputRange1, class InputRange2, class OutputRange, class FT=double>
bool gauss_seidel(const InputRange1& A, const InputRange2& b, OutputRange& x,
                  FT eps = 1e-15, const std::size_t max_iter = 1 << 16) {
  typedef TYPE_FROM_REF(*std::begin(x)) value_type;

  const std::size_t len = std::end(x) - std::begin(x);
  std::vector<value_type> x_past(len);

  std::size_t iter = 0;
  do {
    std::copy(std::begin(x), std::end(x), std::begin(x_past));
    iter++;

    for (std::size_t i = 0; i < len; i++) {
      value_type Ax_i = 0;
      for (std::size_t j = 0; j < len; j++) Ax_i += A[i*len + j] * x[j];
      x[i] = x[i] + 1. / A[i*len + i] * (b[i] - Ax_i);
    }
  } while (euclidean_distance(std::begin(x), std::end(x), std::begin(x_past)) >
               eps &&
           iter < max_iter);

  return iter < max_iter;
}

}  // namespace particles
