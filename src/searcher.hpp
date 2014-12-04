/**
 * @file searcher.hpp
 *
 * @brief spacial search classes
 */

#pragma once

#include "particle.hpp"

#include <utility>
#include <vector>

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

template <class T, std::size_t N>
class DelauneySearcher;

template <class T>
class DelauneySearcher<T, 3> : public SearcherBase<T, 3> {
  typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
  typedef CGAL::Triangulation_vertex_base_with_info_3<std::size_t, K> Vb;
  typedef CGAL::Triangulation_data_structure_3<Vb> Tds;
  typedef CGAL::Delaunay_triangulation_3<K, Tds, CGAL::Fast_location> Delaunay;
  typedef Delaunay::Point Point;
  typedef typename SearcherBase<T, 3>::particle_type particle_type;
  typedef typename SearcherBase<T, 3>::adjacency_list_type adjacency_list_type;

 public:
  DelauneySearcher<T, 3>() : delauney_() {}

  void search(adjacency_list_type& adjacency_list,
              const std::vector<particle_type>& particles) {
    triangulation(particles.begin(), particles.end());

    std::vector<Delaunay::Vertex_handle> adjacent_vertices(particles.size());

    // Loop for all vertices
    auto vit = delauney_.finite_vertices_begin();
    while (vit != delauney_.finite_vertices_end()) {
      auto& neighbors = adjacency_list[vit->info()];
      neighbors.clear();

      // Loop for adjacent vertices of the vertex
      auto adjacent_end =
          delauney_.finite_adjacent_vertices(vit, adjacent_vertices.begin());
      auto it = adjacent_vertices.begin();
      while (it != adjacent_end) {
        neighbors.push_back(&particles[0]);
        ++it;
      }
      ++vit;
    }
  }

 private:
  Delaunay delauney_;

  template <class Iterator>
  void triangulation(Iterator first, Iterator last) {
    std::vector<std::pair<Point, std::size_t>> point_info;
    std::size_t index = 0;
    for (Iterator it = first; it != last; ++it) {
      const auto& pos = it->position();
      // TODO: create vec without specify members x, y, z
      point_info.emplace_back(
          Point(pos[0], pos[1], pos[2]), index++);
    }
    delauney_.clear();
    delauney_.insert(point_info.begin(), point_info.end());
  }
};

}  // namespace search
}  // namespace particles
