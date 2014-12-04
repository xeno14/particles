#include "searcher.hpp"

#include <gtest/gtest.h>

TEST(SearchTest, delaunay) {
  EXPECT_TRUE(true);
  particles::search::DelaunaySearcher<double,3> searcher;
}
