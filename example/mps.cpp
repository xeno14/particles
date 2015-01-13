#include "io.hpp"
#include "mps.hpp"
#include "particle.hpp"
#include "searcher.hpp"

#include <vector>

using namespace particles;

constexpr int DIM = 2;

typedef ParticleWithInfo<double, DIM, int> P;
typedef typename P::vec_type V;

typedef search::SimpleRangeSearch<double, DIM, P> Search;

const int FLUID = 0;
const int WALL  = 1;
const double L0 = 0.025;
const double G = 9.8;
const double RE_N = 2.1*L0;
const double RE_GRAD = 2.1*L0;
const double RE_LAP = 3.1*L0;
const double NU = 1e-6; 
const double dt = 1e-2;


void inital_condition(std::vector<P>& particles) {
  const int L = 64;
  // Set balk
  for (int i = 0; i < L/2; i++)
    for (int j = 0; j < L; j++)
      particles.push_back(P({i * L0, j * L0}, {0, 0}, FLUID));

  // Set wall
  // left
  for (int i=0; i<L+L; i++) {
      particles.push_back(P({-L0, i * L0}, {0, 0}, WALL));
      particles.push_back(P({-2*L0, i * L0}, {0, 0}, WALL));
  }
  // right
  for (int i=0; i<L+L; i++) {
      particles.push_back(P({L*L0, i * L0}, {0, 0}, WALL));
      particles.push_back(P({L*L0+L0, i * L0}, {0, 0}, WALL));
  }
  // bottom
  for (int i=-2; i<L+2; i++) {
      particles.push_back(P({i*L0, -L0}, {0, 0}, WALL));
      particles.push_back(P({i*L0, -2*L0}, {0, 0}, WALL));
  }
}


int main() {
  std::vector<P> particles, particles_mid;
  std::vector<V> accels;
  std::vector<V> viscosity;
  // std::vector<double> pressure;
  V gravity = {0, -G};
  Search n_search(RE_N);
  auto adjacency_list = n_search.create_adjacency_list(particles.size());

  inital_condition(particles);
  n_search.search(adjacency_list, particles);

  auto i0 = particles.size()/2;
  const double n0 = mps::calc_n(particles[i0], adjacency_list[i0], RE_N);
  const double lambda0 = mps::calc_lambda(particles[i0], adjacency_list[i0], RE_N);

  particles_mid.resize(particles.size());
  accels.resize(particles.size());
  viscosity.resize(particles.size());

    io::output_particles(std::cout, particles.begin(), particles.end());
    std::cout << "\n\n";

  for (double t = 0; t <= 0.5; t += dt) {
    std::cerr <<"t="<<t<<"\n";
    n_search.search(adjacency_list, particles);
    mps::calc_viscosity(particles.begin(), particles.end(),
                        adjacency_list.begin(), viscosity.begin(), RE_LAP,
                        2 * NU * DIM / n0 / lambda0);

    for (std::size_t i = 0; i < accels.size(); i++) {
      if (particles[i].info() == FLUID) {
        accels[i] = viscosity[i] + gravity;
      } else {
        accels[i].fill(0);
      }
    }
    std::cerr << accels[0] << std::endl;

    for (std::size_t i = 0; i < accels.size(); i++) {
      accels[i] = viscosity[i] + gravity;
      if (particles[i].info() == FLUID) {
        particles_mid[i].velocity() = particles[i].velocity() + accels[i] * dt;
        particles_mid[i].position() =
            particles[i].position() + particles_mid[i].velocity() * dt;
      }
    }

    for (std::size_t i = 0; i < particles.size(); i++) {
      if (particles[i].info() == FLUID) {
        particles[i].velocity() = particles_mid[i].velocity();
        particles[i].position() = particles_mid[i].position();
        // particles[i].position() =
        //     particles_mid[i].position() +
        //     (particles[i].velocity() - particles_mid[i].velocity()) * dt;
      }
    }

    io::output_particles(std::cout, particles.begin(), particles.end());
    std::cout << "\n\n";
  }

  return 0;  
}
