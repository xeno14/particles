#include "particles/searcher.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

using namespace particles;

typedef Particle<double, 2> P2;
typedef Particle<double, 3> P3;

std::vector<P2> read_particles2(
    const std::string& filename) {
  std::ifstream ifs(filename);
  std::vector<P2> res;
  double x,y;

  while (ifs >> x >> y) {
    Vec<double, 2> pos{x, y};
    Vec<double, 2> vel;
    res.emplace_back(pos, vel);
  }
  return res;
}

std::vector<P3> read_particles3(
    const std::string& filename) {
  std::ifstream ifs(filename);
  std::vector<P3> res;
  double x,y,z;

  while(ifs >> x>>y>>z) {
    Vec<double, 3> pos {x,y,z};   
    Vec<double, 3> vel;   
    res.emplace_back(pos,vel);
  }
  return res;
}

TEST(SearchTest, distance_within) {
  std::vector<P2> particles;
  particles.push_back(P2{0,0});
  particles.push_back(P2{1,0});
  particles.push_back(P2{0,1});
  particles.push_back(P2{0.1,0});

  std::vector<P2*> candidates;
  for (auto& p : particles) candidates.push_back(&p);
  auto last = search::distance_within(candidates, particles[0], 0.5);

  EXPECT_EQ(2, last - candidates.begin());
}

TEST(SearchTest, nearest) {
  std::vector<P2> particles;
  particles.push_back(P2{0,0});
  particles.push_back(P2{0.1,0});
  particles.push_back(P2{0.2,1});
  particles.push_back(P2{0.3,0});

  std::vector<P2*> candidates;
  for (auto& p : particles) candidates.push_back(&p);

  auto last = search::nearest(candidates, particles[0], 2);

  EXPECT_EQ(2, last - candidates.begin());
  EXPECT_DOUBLE_EQ(0, candidates[0]->position()[0]);
  EXPECT_DOUBLE_EQ(0.1, candidates[1]->position()[0]);
}

TEST(SearchTest, SimpleRangeSearch) {
  search::SimpleRangeSearch<double, 2> searcher(1.001);
  typename decltype(searcher)::adjacency_list_type adjacency_list;

  std::vector<P2> particles;
  particles.push_back(P2{0,0});
  particles.push_back(P2{1,0});
  particles.push_back(P2{0,1});
  particles.push_back(P2{0.1,0});

  searcher.search(adjacency_list, particles);
  EXPECT_EQ(4, adjacency_list[0].size());
  EXPECT_EQ(3, adjacency_list[1].size());
  EXPECT_EQ(2, adjacency_list[2].size());
  EXPECT_EQ(3, adjacency_list[3].size());
}

TEST(SearchTest, delaunay2) {
  auto particles = read_particles2("../../test/data/2d.xyz");
  ASSERT_TRUE(particles.size()>0);

  search::DelaunaySearcher<double,2> searcher;
  auto adjacency_list = searcher.create_adjacency_list();
  searcher.search(adjacency_list, particles);

  EXPECT_EQ(3, adjacency_list[0].size());
  EXPECT_EQ(2, adjacency_list[1].size());
  EXPECT_EQ(4, adjacency_list[2].size());
  EXPECT_EQ(3, adjacency_list[3].size());
  EXPECT_EQ(4, adjacency_list[4].size());
  EXPECT_EQ(4, adjacency_list[5].size());
}

TEST(SearchTest, delaunay3) {
  // Assume that cwd is (project root)/build/test
  auto particles = read_particles3("../../test/data/3d.xyz");
  ASSERT_TRUE(particles.size()>0);

  search::DelaunaySearcher<double,3> searcher;
  typename decltype(searcher)::adjacency_list_type adjacency_list;
  searcher.search(adjacency_list, particles);

  EXPECT_EQ(4, adjacency_list[0].size());
  EXPECT_EQ(6, adjacency_list[1].size());
  EXPECT_EQ(5, adjacency_list[2].size());
  EXPECT_EQ(6, adjacency_list[3].size());
  EXPECT_EQ(4, adjacency_list[4].size());
  EXPECT_EQ(4, adjacency_list[5].size());
  EXPECT_EQ(5, adjacency_list[6].size());
}

TEST(SearchTest, kdtree) {
  std::vector<P2> particles;
  for(int i=0; i<100000; i++) particles.push_back(P2({(double)i,0},{0,0}));

  search::KdTreeSearcher<double, 2> searcher(1.1);
  auto adjacency_list = searcher.create_adjacency_list(particles.size());
  searcher.search(adjacency_list, particles);

  EXPECT_EQ(2, adjacency_list[0].size());
  EXPECT_EQ(3, adjacency_list[1].size());
  EXPECT_EQ(3, adjacency_list[2].size());
  EXPECT_EQ(3, adjacency_list[3].size());
}
