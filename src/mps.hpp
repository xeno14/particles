/**
 * @file mps.hpp
 *
 * @brief Moving Particle Semi-implicit
 */

#pragma once

#include <algorithm>
#include <vector>

namespace particles {
namespace mps {

template <class T>
T weight(T r, T re) {
  if (r == 0) return 0;
  if (r >= re) return 0;
  return re/r - 1;
}

template <class P, class FT=double>
FT calc_n(const P& p, const std::vector<const P*>& neighbors, FT re) {
  FT n = 0;
  for (const auto* q: neighbors) {
    n += weight(p.position().distance(q->position()), re);
  }
  return n;
}

template <class P, class FT=double>
FT calc_lambda(const P& p, const std::vector<const P*>& neighbors, FT re) {
  FT numer=0;
  FT denom=0;
  for (const auto* q : neighbors) {
    auto r = q->distance(p);
    if (q != &p) {
      numer += r * r * weight(r, re);
      denom += weight(r, re);
    }
  }
  return denom/numer;
}


/**
 *
 * \f[
 *  c = \frac{2d}{n_0 \lambda_0}
 * \f]
 *
 * @param result vec list
 * @param first1 particles
 * @param last1 particles
 * @param first2 adjacency_list
 */
template <class InputIterator1, class InputIterator2, class OutputIterator,
          class FT = double>
OutputIterator calc_viscosity(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, OutputIterator result,
                              FT re, FT coeff) {
  while (first1 != last1) {
    const auto& p = *first1;
    const auto& neighbors = *first2;
    result->fill(0);
    for (const auto* q : neighbors) {
      if (&p != q) {
        *result += (q->velocity() - p.velocity()) * weight(q->distance(p), re);
      }
      *result *= coeff;
    }
    result++; first1++; first2++;
  }
  return result;
}


}  // namespace mps
}  // namespace particles
