/**
 * @file delaunay_search.hpp
 *
 * @brief implemention of delaunay_search (wrapping CGAL/Delaunay triangulation)
 */

#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

namespace particles {
namespace search {
namespace internal {

/**
 * @brief creates Point from Vec<T,N>
 * @todo call constructor using tuple expansion
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

}  // namespace internal
}  // namespace search
}  // namespace particles
