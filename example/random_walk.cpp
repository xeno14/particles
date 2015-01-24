#include "particles/particles.hpp"

#include <iostream>

const double dt = 1e-6;

int main() {
  using namespace particles;

  Particle<double, 2> p;
  random::UniformOnSphere<double, 2> rand(1.0);
  rand.seed_dev();

  for (int i=0; i<100000; i++) {
    p.velocity()  = rand();
    p.position() += p.velocity() * dt; 

    std::cout << p.position(0) << " " << p.position(1) << std::endl;
  }
  return 0;
}
