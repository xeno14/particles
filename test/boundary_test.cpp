#include "boundary.hpp"
#include "particle.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <vector>

using namespace particles;

typedef Particle<double, 2> P2;

TEST(BoundaryTest, FreeBoundary) {
  P2 p({1,2},{3,4});

  boundary::FreeBoundary<double, 2> b;
  b.apply(p.position());

  EXPECT_DOUBLE_EQ(1, p.position(0));
  EXPECT_DOUBLE_EQ(2, p.position(1));
}

TEST(BoundaryTest, PeriodicBoundary) {
  P2 p;

  boundary::PeriodicBoundary<double, 2> pb(0., 1., 0., 2.);

  p.position(0) = 1.2;
  p.position(1) = -1.1;
  pb.apply(p.position());
  EXPECT_DOUBLE_EQ(0.2, p.position(0));
  EXPECT_DOUBLE_EQ(0.9, p.position(1));

  /** @todo fail: actual=0.20000000000000018 */
  // p.position(0) = 2.2;
  // pb.apply(p.position());
  // EXPECT_DOUBLE_EQ(0.2, p.position(0));
}

TEST(BoundaryTest, PeriodicBoundaryConstructor) {
  P2 p;

  // Square with length 1.0
  boundary::PeriodicBoundary<double, 2> pb(1.);

  p.position(0) = 1.2;
  p.position(1) = -1.1;
  pb.apply(p.position());
  EXPECT_DOUBLE_EQ(0.2, p.position(0));
  EXPECT_DOUBLE_EQ(0.9, p.position(1));
}
