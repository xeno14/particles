/**
 * @file searcher.hpp
 *
 * @brief spacial search classes
 *
 * @todo pipe-line search
 */

#pragma once

#include "particle.hpp"
#include "range.hpp"
#include "details/delaunay_search.hpp"
#include "details/kdtree_search.hpp"

#include <utility>
#include <vector>


namespace particles {
namespace search {

/**
 * @brief find particles distance within r
 *
 * @todo floating point exception??
 */
template <class Range, class T, std::size_t N>
auto distance_within(Range& candidates, const Particle<T, N>& p, const T r) {
  auto last = std::remove_if(std::begin(candidates), std::end(candidates),
                             [&p, r](const Particle<T, N>* q) {
    return p.squared_distance(q) > r * r;
  });
  candidates.resize(last - candidates.begin());
  return last;
}

/**
 * @brief find n nearest particles
 *
 * @param candidates list of pointer to particles
 */
template <class Range, class T, std::size_t N>
auto nearest(Range& candidates, const Particle<T, N>& p,
             const std::size_t n = 1) {
  typedef Particle<T, N> P;

  // sort by distance from p
  std::sort(std::begin(candidates), std::end(candidates),
            [&p](const P* p1, const P* p2) {
    return p.squared_distance(p1) < p.squared_distance(p2);
  });
  const auto sz =
      std::min<std::size_t>(n, std::end(candidates) - std::begin(candidates));
  candidates.resize(sz);
  return candidates.end();
}

template <class T, std::size_t N>
class SearcherBase {
 public:
  typedef Particle<T, N> particle_type;
  typedef std::vector<std::vector<const particle_type*>> adjacency_list_type;

  virtual ~SearcherBase() {}
  /**
   * @todo use iterator
   */
  virtual void search(adjacency_list_type& adjacency_list,
                      const std::vector<particle_type>& particles) = 0;

  /**
   * @brief Create adjacency_list_type array with size n
   * @param n size
   */
  auto create_adjacency_list(const std::size_t n=0) {
    return adjacency_list_type(n);
  }
};


/**
 * Compare all distances between all pairs of particles.
 *
 * Complexity: \f$O(n^2)\f$
 *
 * Memory: \f$O(1)\f$
 *
 * @brief simple range search for N-dimension
 * @tparam T floating point
 * @tparam N dimension
 */
template <class T, std::size_t N>
class SimpleRangeSearch : public SearcherBase<T,N> {
 public:
  typedef typename SearcherBase<T, N>::particle_type particle_type;
  typedef typename SearcherBase<T, N>::adjacency_list_type adjacency_list_type;

  SimpleRangeSearch(T d) : distance_(d) {}

  void search(adjacency_list_type& adjacency_list,
              const std::vector<particle_type>& particles) {
    adjacency_list.resize(particles.size());
    for (auto l : adjacency_list) l.clear();

    for (std::size_t i = 0; i < particles.size(); i++) {
      adjacency_list[i].push_back(&particles[i]);
      for (std::size_t j = i+1; j < particles.size(); j++) {
        const auto& pi = particles[i].position();
        const auto& pj = particles[j].position();
        if (pi.squared_distance(pj) <= distance_ * distance_) {
          adjacency_list[i].push_back(&particles[j]);
          adjacency_list[j].push_back(&particles[i]);
        }
      }
    }
  }

 private:
  const T distance_;
};

/**
 * @brief searchs adjacencies using Delaunay triangulation
 * @tparam T floating point
 * @tparam N dimension
 */
template <class T, std::size_t N>
class DelaunaySearcher : public SearcherBase<T, N> {
  typedef typename internal::DelaunayType<T, N>::type Delaunay;

 public:
  typedef typename SearcherBase<T, N>::particle_type particle_type;
  typedef typename SearcherBase<T, N>::adjacency_list_type adjacency_list_type;

  DelaunaySearcher<T, N>() : delaunay_() {}

  void search(adjacency_list_type& adjacency_list,
              const std::vector<particle_type>& particles) {
    internal::DelaunaySearchImpl<Delaunay, T, N>::search(
        delaunay_, adjacency_list, particles);
  }

 private:
  Delaunay delaunay_;
};

/**
 * @brief searchs adjacencies using KdTree
 *
 * Pick particles with distance less than \f$r\f$
 *
 * @tparam T floating point
 * @tparam N dimension
 */
template <class T, std::size_t N>
class KdTreeSearcher : public SearcherBase<T, N> {
 public:
  typedef typename SearcherBase<T, N>::particle_type particle_type;
  typedef typename SearcherBase<T, N>::adjacency_list_type adjacency_list_type;

  KdTreeSearcher(T r) : r_(r) {}

  void search(adjacency_list_type& adjacency_list,
              const std::vector<particle_type>& particles) {
    internal::KdTreeSearchImpl<T, N>::search(adjacency_list, particles, r_);
  }

  /** @brief set searching radious */
  void set_r(T r) { r_ = r; }

 private:
  T r_;
};

}  // namespace search
}  // namespace particles
