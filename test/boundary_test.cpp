#include "boundary.hpp"
#include "particle.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <vector>

typedef particles::Particle<double, 2> P2;

TEST(BoundaryTest, PeriodicBoundary) {
  P2 p;

  particles::boundary::PeriodicBoundary<double, 0> pb0(0, 1);
  particles::boundary::PeriodicBoundary<double, 1> pb1(0, 2);

  p.position(0) = 1.2;
  p.position(1) = -1.1;
  pb0.apply(p.position());
  pb1.apply(p.position());
  EXPECT_DOUBLE_EQ(0.2, p.position(0));
  EXPECT_DOUBLE_EQ(0.9, p.position(1));
  
  /** @todo fail: actual=0.20000000000000018 */
  // p.position(0) = 2.2;
  // pb0.apply(p.position());
  // EXPECT_DOUBLE_EQ(0.2, p.position(0));
}

// TEST(BoundaryTest, PeriodicRect) {
//   P2 p;
//
//   particles::boundary::PeriodicRect<double, 2> pb0(0, 1, 0, 1);
// }
