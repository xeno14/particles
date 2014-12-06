/**
 * @file vicsek2.cpp
 *
 * @brief Simulate Vicsek model in 2D with periodic boundary condition
 *
 * @todo wrtie explanation about Vicsek model
 */

#include "boundary.hpp"
#include "io.hpp"
#include "particle.hpp"
#include "random.hpp"
#include "searcher.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

using namespace particles;

int main() {
  std::ofstream fout("vicsek2.dat");  // Output to a file

  const int N = 128;          // Number of particles
  const double v0 = 0.1;      // Velocity of particles
  const double L = 16;        // System size
  const double eta = v0 / 5;  // Strength of noise
  const double r0 = 1.0;      // Interaction range

  // Boundary condition: periodic boundary for position
  boundary::PeriodicBoundary<double, 2> boundary(0, L, 0, L);

  // Search: interaction with particles within distamce r0
  search::SimpleRangeSearch<double, 2> searcher(r0);

  // List of pointers to particles interact with
  typename decltype(searcher)::adjacency_list_type adjacency_list;

  std::vector<Particle<double, 2>> particles(N);
  std::vector<Particle<double, 2>> new_particles(N);  // Store next step

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

  // Time evolution!!
  for (int t = 0; t < 1000; ++t) {
    // Create adjacency list for all particles
    searcher.search(adjacency_list, particles);

    // for (std::size_t i = 0; i < particles.size(); ++i) {
    ENUMERATE_BEGIN(i, p, particles) {
      const auto& x = p.position();
      const auto& v = p.velocity();
      auto& nx = new_particles[i].position();
      auto& nv = new_particles[i].velocity();

      // Velocity at next step
      // Get average velocity among neighbors
      for (const auto* q : adjacency_list[i]) {
        nv += q->velocity();
      }
      nv /= adjacency_list[i].size();

      // Add noise
      nv = nv + random::UniformRand<double>::get_vec();

      // Set new values
      nx = x + v;
      nv.normalize(v0);
    } ENUMERATE_END;

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
