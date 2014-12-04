/**
 * @file searcher.hpp
 *
 * @brief spacial search classes
 */

#pragma once

#include "particle.hpp"

#include <utility>
#include <vector>

#include<iostream>
#include<typeinfo>

#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

namespace particles {
namespace search {

template <class T, std::size_t N>
class SearcherBase {
 public:
  typedef Particle<T, N> particle_type;
  typedef std::vector<std::vector<const particle_type*>> adjacency_list_type;

  virtual ~SearcherBase() {}
  /**
   * @TODO use iterator
   */
  virtual void search(adjacency_list_type& adjacency_list,
                      const std::vector<particle_type>& particles) = 0;
};

namespace {

/**
 * @TODO call constructor using tuple expansion
 * @brief generates Point from Vec<T,N>
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
 * @brief Executes triangulation and creates adjacency list
 * @tparam Delaunay triangulation
 * @tparam T floating point
 * @tparam N dimension
 */
template <class Delaunay, class T, std::size_t N>
struct DelaunaySearchImpl {
  typedef typename Delaunay::Point Point;
  typedef typename Delaunay::Vertex_handle Vertex_handle;

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
    std::size_t index = 0;
    for (auto it = particles.begin(); it != particles.end(); ++it) {
      const auto& pos = it->position();
      auto point = VecToPoint<Point,T,N>::generate(pos);
      point_info.emplace_back(point, index++);
    }
    delaunay.clear();
    delaunay.insert(point_info.begin(), point_info.end());

    // Set adjacent vertices
    adjacency_list.resize(particles.size());
    std::vector<Vertex_handle> adjacent_vertices(particles.size());
    // // Loop for all vertices
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
};

}  // namespace

template <class T, std::size_t N>
class DelaunaySearcher;

template <class T>
class DelaunaySearcher<T, 3> : public SearcherBase<T, 3> {
  typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
  typedef CGAL::Triangulation_vertex_base_with_info_3<std::size_t, K> Vb;
  typedef CGAL::Triangulation_data_structure_3<Vb> Tds;
  typedef CGAL::Delaunay_triangulation_3<K, Tds, CGAL::Fast_location> Delaunay;
  typedef Delaunay::Point Point;

 public:
  typedef typename SearcherBase<T, 3>::particle_type particle_type;
  typedef typename SearcherBase<T, 3>::adjacency_list_type adjacency_list_type;

  DelaunaySearcher<T, 3>() : delaunay_() {}

  void search(adjacency_list_type& adjacency_list,
              const std::vector<particle_type>& particles) {
    DelaunaySearchImpl<Delaunay, T, 3>::search(delaunay_, adjacency_list,
                                               particles);
  }

 private:
  Delaunay delaunay_;
};

}  // namespace search
}  // namespace particles
