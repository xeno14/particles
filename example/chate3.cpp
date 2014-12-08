#include "boundary.hpp"
#include "io.hpp"
#include "particle.hpp"
#include "random.hpp"
#include "searcher.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

using namespace particles;

typedef Particle<double, 3> P;

const auto& get_v(const P* p) { return p->velocity(); }

int main() {
  std::ofstream fout("chate3.dat");

  const int N = 128;          // Number of particles
  const double v0 = 0.1;      // Velocity of particles
  const double L = 16;        // System size
  const double eta = v0 / 5;  // Strength of noise
  const double r0 = 5;        // Interaction range
  const int Q = 7;            // number of interactions

  boundary::PeriodicBoundary<double, 3> boundary(0, L, 0, L, 0, L);
  search::DelaunaySearcher<double, 3> searcher;
  typename decltype(searcher)::adjacency_list_type adjacency_list;
  std::vector<P> particles(N);
  std::vector<P> new_particles(N);  // Store next step

  // Initial condition: set position and velocity randomly
  random::UniformRand<double>::set_range(0, L);
  for (auto& p : particles) {
    p.position() = random::UniformRand<double>::get_vec();
  }
  random::UniformRand<double>::set_range(-1, 1);
  for (auto& p : particles) {
    p.velocity() = random::UniformRand<double>::get_vec();
    p.velocity().normalize(v0);
  }

  // Output for first step
  fout << std::scientific;
  io::output_particles(fout, particles.begin(), particles.end(), "\t");
  fout << "\n\n";

  random::UniformRand<double>::set_range(-eta, eta);

  for (int t=0; t<1000; ++t) {
    searcher.search(adjacency_list, particles);

    for (auto e : range::enumerate(particles)) {
      auto  i = e.first;
      auto& p = e.second;

      const auto& x = p.position();
      const auto& v = p.velocity();
      auto& nx = new_particles[i].position();
      auto& nv = new_particles[i].velocity();
      const auto& interactors = adjacency_list[i];  // particles interact with i
      auto& neighbors = adjacency_list[i];

      // search::distance_within(neighbors, p, r0);
      search::nearest(neighbors, p, Q);

      // Position at next step
      nx = x + v;

      // Velocity at next step
      // Get average velocity among neighbors
      nv = range::average(range::convert_iterator(neighbors.begin(), get_v),
                          range::convert_iterator(neighbors.end(),   get_v)) +
           random::UniformRand<double>::get_vec();
      nv.normalize(v0);
    }
    // Renew position and velocity of all particles
    for (std::size_t i = 0; i < particles.size(); ++i) {
      particles[i] = new_particles[i];
    }

    // Apply boundary condition
    for (auto& p : particles) {
      boundary.apply(p.position());
    }

    // Output each step
    io::output_particles(fout, particles.begin(), particles.end(), "\t");
    fout << "\n\n";
  }

  return 0;
}
