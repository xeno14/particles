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

#include <utility>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

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

namespace internal {

/**
 * @todo call constructor using tuple expansion
 * @brief creates Point from Vec<T,N>
 */
template <class Point, class T, std::size_t N>
struct VecToPoint;

template <class Point, class T>
struct VecToPoint<Point, T, 2> {
  static Point generate(const Vec<T, 2>& vec) {
    return Point(vec[0], vec[1]);
  }
};

template <class Point, class T>
struct VecToPoint<Point, T, 3> {
  static Point generate(const Vec<T, 3>& vec) {
    return Point(vec[0], vec[1], vec[2]);
  }
};

/**
 * @brief Walk adjacent vertices in 2d
 */
template <std::size_t N, class Delaunay, class AdjacencyList, class Particles>
typename std::enable_if<N==2, void>::type
  walk_adjacent_vertices(
      Delaunay& delaunay, AdjacencyList& adjacency_list, Particles& particles) {
    auto vit = delaunay.finite_vertices_begin();
    while (vit != delaunay.finite_vertices_end()) {
      auto& neighbors = adjacency_list[vit->info()];
      neighbors.clear();

      // Loop for adjacent vertices in circular way
      auto vc = delaunay.incident_vertices(vit);
      decltype(vc) done = vc;
      if (vc != 0) {
        do {
          if (vc->info() < particles.size()) {  // validation for the vertex
            neighbors.push_back(&(particles[vc->info()]));
          }
        } while(++vc != done);
      }
      ++vit;
    }
  }

/**
 * @brief Walk adjacent vertices in 3d
 */
template <std::size_t N, class Delaunay, class AdjacencyList, class Particles>
typename std::enable_if<N==3, void>::type
  walk_adjacent_vertices(
      Delaunay& delaunay, AdjacencyList& adjacency_list, Particles& particles) {
    typedef typename Delaunay::Vertex_handle Vertex_handle;

    std::vector<Vertex_handle> adjacent_vertices(particles.size());
    // Loop for all vertices
    auto vit = delaunay.finite_vertices_begin();
    while (vit != delaunay.finite_vertices_end()) {
      auto& neighbors = adjacency_list[vit->info()];
      neighbors.clear();

      // Loop for adjacent vertices of the vertex
      auto adjacent_end =
          delaunay.finite_adjacent_vertices(vit, adjacent_vertices.begin());
      auto it = adjacent_vertices.begin();
      while (it != adjacent_end) {
        neighbors.push_back(&(particles[(*it)->info()]));
        ++it;
      }
      ++vit;
    }
  }

/**
 * @brief Executes triangulation and creates adjacency list
 * @tparam Delaunay triangulation
 * @tparam T floating point
 * @tparam N dimension
 */
template <class Delaunay, class T, std::size_t N>
struct DelaunaySearchImpl {
  typedef typename Delaunay::Point Point;

  /**
   * @brief execute searching
   * @tparam AdjacencyList
   * @tparam Iterator random access iterator
   */
  template <class AdjacencyList, class Particles>
  static void search(Delaunay& delaunay, AdjacencyList& adjacency_list,
                     const Particles& particles) {
    // Triangulation
    std::vector<std::pair<Point, std::size_t>> point_info;
    for (std::size_t i=0; i<particles.size(); i++) {
      const auto& pos = particles[i].position();
      auto point = VecToPoint<Point,T,N>::generate(pos);
      point_info.emplace_back(point, i);
    }
    delaunay.clear();
    delaunay.insert(point_info.begin(), point_info.end());

    // Set adjacent vertices
    adjacency_list.resize(particles.size());
    walk_adjacent_vertices<N>(delaunay, adjacency_list, particles);
  }
};


/**
 * @brief Type of delaunay triangulation class.
 */
template <class T, std::size_t N>
struct DelaunayType;

template <class T>
struct DelaunayType<T, 3> {
  typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
  typedef CGAL::Triangulation_vertex_base_with_info_3<std::size_t, K> Vb;
  typedef CGAL::Triangulation_data_structure_3<Vb>                    Tds;
  typedef CGAL::Delaunay_triangulation_3<K, Tds, CGAL::Fast_location> Delaunay;
  typedef Delaunay type;
};

template <class T>
struct DelaunayType<T, 2> {
  typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
  typedef CGAL::Triangulation_vertex_base_with_info_2<std::size_t, K> Vb;
  typedef CGAL::Triangulation_data_structure_2<Vb>                    Tds;
  typedef CGAL::Delaunay_triangulation_2<K, Tds>                      Delaunay;
  typedef Delaunay type;
};

template <class T, std::size_t N>
struct KdTreeSearchImpl {

  template <class AdjacencyList, class Particles>
  static void search(AdjacencyList& adjacency_list, const Particles& particles) {

  }
};

}  // namespace internal


/**
 * @todo 2-d
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

template <class T, std::size_t N>
class KdTreeSearcher : public SearcherBase<T, N> {
 public:
  typedef typename SearcherBase<T, N>::particle_type particle_type;
  typedef typename SearcherBase<T, N>::adjacency_list_type adjacency_list_type;

  KdTreeSearcher(T r) : r_(r) {}

  void search(adjacency_list_type& adjacency_list,
              const std::vector<particle_type>& particles) {
    internal::KdTreeSearchImpl<T, N>::search(adjacency_list, particles);
  }

 private:
  T r_;
};

}  // namespace search
}  // namespace particles
