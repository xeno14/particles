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

template <class T, class N>
class SearcherBase {
  typedef Particle<T, N> particle_type;
  typedef std::vector<std::vector<particle_type*>> adjacency_list_type;

 public:
  virtual ~SearcherBase() {}
  /**
   * @TODO use iterator
   */
  virtual void search(adjacency_list_type& adjacency_list,
                      const std::vector<Particle>& particles) = 0;
};

template <class T, class N>
class DelauneySearcher;

template <class T>
class DelauneySearcher<T, 3> : public SearcherBase<T, 3> {
  typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
  typedef CGAL::Triangulation_vertex_base_with_info_3<std::size_t, K> Vb;
  typedef CGAL::Triangulation_data_structure_3<Vb> Tds;
  typedef CGAL::Delaunay_triangulation_3<K, Tds, CGAL::Fast_location> Delaunay;
  typedef Delaunay::Point Point;

 public:
  template <class Iterator>
  DelaunaySearcher(Iterator first, Iterator last)
      : delauney_(), point_info_() {
    // TODO: save reference to particles here
    std::size_t index = 0;
    for (Iterator it = first; it != last; ++it) {
      const auto& pos = it->position();
      // TODO: create vec without specify members x, y, z
      point_info_.emplace_back(
          Point(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos)), index++);
    }
  }

  void search(adjacency_list_type& adjacency_list,
              const std::vector<Particle>& particles) {
    triangulation();
    auto vit = delauney_.finite_vertices_begin();
    adjacent_vertices_.resize(particles.size());
    while(vit != delauney_.finite_vertices_end()) {
      auto& neighbors = adjacency_list[vit->info()];
      neighbors.clear();

      auto adjacent_end =
          delauney_.finite_adjacent_vertices(vit, adjacent_vertices_.begin());
      for (auto it = adjacent_vertices_.begin(), it != adjacent_end; ++it ) {
        neighbors.push_back(&particles[(*it)->info()]);
      }

      ++vit;
    }
  }

 private:
  Delaunay delauney_;
  std::vector<std::pair<Point, std::size_t>> point_info_;
  std::vector<Delaunay::Vertex_handle> adjacent_vertices_;

  void triangulation() {
    delauney_.clear();
    delauney_.insert(point_info_.begin(), point_info_.end());
  }
};

}  // namespace search
}  // namespace particles
