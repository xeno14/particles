#pragma once

#include "../range.hpp"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/basic.h>
#include <CGAL/Cartesian_d.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/property_map.h>
#include <boost/iterator/zip_iterator.hpp>
#include <utility>
#include <CGAL/Kd_tree.h>
#include <CGAL/Fuzzy_sphere.h>
#include <CGAL/Fuzzy_iso_box.h>
#include <CGAL/Search_traits_d.h>

namespace particles {
namespace search {
namespace internal {

template <class T, std::size_t N>
struct KdTreeSearchImpl {
  typedef CGAL::Cartesian_d<double> K;
  typedef K::Point_d Point_d;
  typedef boost::tuple<Point_d, std::size_t> Point_and_index;

  typedef CGAL::Search_traits_d<K> Traits_base;
  typedef CGAL::Search_traits_adapter<
      Point_and_index, CGAL::Nth_of_tuple_property_map<0, Point_and_index>,
      Traits_base> Traits;
  typedef CGAL::Kd_tree<Traits> Tree;
  typedef CGAL::Fuzzy_sphere<Traits> Fuzzy_sphere;

  static Point_d VecToPoint_d(const Vec<T, N>& v) {
    return Point_d(N, v.cbegin(), v.cend());
  }

  template <class AdjacencyList, class Particles>
  static void search(AdjacencyList& adjacency_list, const Particles& particles,
                     const T r) {
    std::vector<std::size_t> indices;
    std::vector<Point_d> points;

    for (std::size_t i = 0; i < particles.size(); i++) {
      indices.push_back(i);
      points.push_back(VecToPoint_d(particles[i].position()));
    }

    Tree tree(boost::make_zip_iterator(
                  boost::make_tuple(points.begin(), indices.begin())),
              boost::make_zip_iterator(
                  boost::make_tuple(points.end(), indices.end())));

    std::vector<Point_and_index> result;
    for (std::size_t i = 0; i < particles.size(); i++) {
      result.clear();
      adjacency_list[i].clear();

      Fuzzy_sphere query(VecToPoint_d(particles[i].position()), r);
      tree.search(range::push_back_iterator(result), query);

      for (const auto& t : result) {
        adjacency_list[i].push_back(&particles[t.get<1>()]);
      }
    }
  }
};

}  // namespace internal
}  // namespace search
}  // namespace particles
