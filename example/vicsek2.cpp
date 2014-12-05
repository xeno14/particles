#include "boundary.hpp"
#include "io.hpp"
#include "particle.hpp"
#include "random.hpp"
#include "searcher.hpp"

#include <iostream>
#include <vector>

using namespace particles;

typedef Particle<double, 2> P2;
typedef boundary::PeriodicBoundary<double, 2> Boundary;
typedef search::SimpleRangeSearch<double, 2> Search;

int main() {
  const int N = 128;
  const double v0 = 0.1;
  const double L = 16;
  const double eta = v0/5;
  Boundary boundary(0, L, 0, L);
  Search searcher(1.0);
  typename Search::adjacency_list_type adjacency_list;
  
  std::vector<P2> particles(N);

  for(auto& p : particles) {
    random::UniformRand<double>::set_range(0, L);
    p.position(0) = random::UniformRand<double>::get();
    p.position(1) = random::UniformRand<double>::get();
    random::UniformRand<double>::set_range(-1, 1);
    p.velocity(0) = random::UniformRand<double>::get();
    p.velocity(1) = random::UniformRand<double>::get();
    p.velocity().normalize(v0);
  }

  io::output_particles(std::cout, particles.begin(), particles.end());

  random::UniformRand<double>::set_range(-eta, eta);

  for (int t=0; t<1000; ++t) {
    searcher.search(adjacency_list, particles);

    for (std::size_t i=0; i< particles.size(); ++i) {
      auto& p = particles[i];
      const auto& x = p.position();
      const auto& v = p.velocity();
      auto nx = p.position();
      auto nv = p.velocity();

      // Calculate position and velocity at next step
      for(const auto* q : adjacency_list[i]) {
        nv += q->velocity();
      }
      nv /= adjacency_list[i].size();
      nv(0) += random::UniformRand<double>::get();
      nv(1) += random::UniformRand<double>::get();

      nx = x + v;
      nv.normalize(v0);

      // Set new values
      p.position() = nx;
      p.velocity() = nv;
    }

    for(auto& p : particles) {
      boundary.apply(p.position());
    }

    io::output_particles(std::cout, particles.begin(), particles.end());
    std::cout << "\n\n";
  }

  return 0;
}
