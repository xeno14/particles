#include "boundary.hpp"
#include "particle.hpp"
#include "random.hpp"
#include "searcher.hpp"

#include <iostream>
#include <vector>

using particles::random::UniformRand;

typedef particles::Particle<double, 2> P2;
typedef particles::boundary::PeriodicBoundary<double, 2> Boundary;
typedef particles::search::SimpleRangeSearch<double, 2> Search;

template<class Iterator>
void output(Iterator first, Iterator last) {
  auto it = first;
  while(it != last) {
    std::cout << it->position(0) << " " << it->position(1) << " "; 
    std::cout << it->velocity(0) << " " << it->velocity(1) << std::endl; 
    ++it;
  }
}

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
    UniformRand<double>::set_range(0, L);
    p.position(0) = UniformRand<double>::get();
    p.position(1) = UniformRand<double>::get();
    UniformRand<double>::set_range(-1, 1);
    p.velocity(0) = UniformRand<double>::get();
    p.velocity(1) = UniformRand<double>::get();
    p.velocity().normalize(v0);
  }

  output(particles.begin(), particles.end());

  for (int t=0; t<1000; ++t) {
    searcher.search(adjacency_list, particles);

    for (std::size_t i=0; i< particles.size(); ++i) {
      auto& p = particles[i];
      const auto& x = p.position();
      const auto& v = p.velocity();
      auto nx = p.position();
      auto nv = p.velocity();

      for(const auto* q : adjacency_list[i]) {
        nv += q->velocity();
      }
      nv /= adjacency_list[i].size();
      UniformRand<double>::set_range(-eta, eta);
      nv(0) += UniformRand<double>::get();
      nv(1) += UniformRand<double>::get();

      nx = x + v;
      nv.normalize(v0);

      p.position() = nx;
      p.velocity() = nv;
    }

    for(auto& p : particles) {
      boundary.apply(p.position());
    }

    output(particles.begin(), particles.end());
    std::cout << std::endl << std::endl;
  }

  return 0;
}
