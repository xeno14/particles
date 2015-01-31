/**
 * @file vicsek2.cpp
 *
 * @brief Simulate Vicsek model in 2D with periodic boundary condition
 *
 * Build:
 *  mkdir build && cd build
 *  cmake .. && make
 *
 * Start simulation:
 *  example/vicsek2.out
 *
 * Show result:
 *  gnuplot ../example/vicsek2.plt
 *
 * @todo wrtie explanation about Vicsek model
 */

#include "particles/particles.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

using namespace particles;

typedef Particle<double, 2> P;


const int N = 128;          // Number of particles
const double v0 = 0.1;      // Velocity of particles
const double L = 16;        // System size
const double eta = v0 / 5;  // Strength of noise
const double r0 = 1.0;      // Interaction range


/**
 * Initial condition:
 *  position: randomly place in [0,L]^2
 *  velocity: direction is randomly chosen and it absolute value is v0
 */
void initial_condition(std::vector<P>& particles) {
  particles.resize(N);

  // Initial condition: set position and velocity randomly
  random::UniformGenerator<double> pos_gen(0, L);
  random::UniformOnSphere<double, 2> vel_gen(v0);

  pos_gen.seed_dev();
  vel_gen.seed_dev();

  for (auto& p : particles) {
    p.position() = pos_gen();
    p.velocity() = vel_gen();
  }
}


int main() {
  std::ofstream fout("vicsek2.dat");  // Output to a file


  // Boundary condition: periodic boundary for position
  boundary::PeriodicBoundary<double, 2> boundary(0, L, 0, L);

  // Search: interaction with particles within distamce r0
  search::KdTreeSearcher<double, 2> searcher(r0);

  // List of pointers to particles within the interect range
  auto adjacency_list = searcher.create_adjacency_list();

  std::vector<P> particles(N);
  std::vector<P> new_particles(N);  // Store next step

  initial_condition(particles);

  // Output for first step
  fout << "x" << "\t" << "y" << "\t" << "u" << "\t" << "v" << std::endl;
  fout << std::scientific;
  io::output_particles(fout, particles.begin(), particles.end(), "\t")
      << "\n\n";

  random::UniformOnSphere<double, 2> eta_gen(eta);
  eta_gen.seed_dev();

  // Time evolution!!
  for (int t = 0; t < 3000; ++t) {
    std::cout << "t=" << t << std::endl;

    // Create adjacency list for all particles
    searcher.search(adjacency_list, particles);

    for (auto e : enumerate(particles)) {
      auto  i = e.first;    // index of the particle
      auto& p = e.second;   // i-th particle
      const auto& x = p.position();
      const auto& v = p.velocity();
      auto& nx = new_particles[i].position();
      auto& nv = new_particles[i].velocity();
      const auto& neighbors = adjacency_list[i];

      // Position at next step
      nx = x + v;

      // Velocity at next step
      // Get average velocity over neighbors
      auto iter = transform_iterator(neighbors.begin(), neighbors.end(),
                                     [](auto* p) { return p->velocity(); });
      nv = average(iter.first, iter.second) + eta_gen();
      nv.normalize(v0);
    };

    // Renew position and velocity of all particles
    particles.swap(new_particles);

    // Apply boundary condition
    for (auto& p : particles) boundary.apply(p);

    // Output each step
    io::output_particles(fout, particles.begin(), particles.end(), "\t")
        << "\n\n";
  }

  return 0;
}
