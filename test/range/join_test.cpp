#include <gtest/gtest.h>

#include "particles/range/join.hpp"

using namespace particles;
using namespace particles::range;


class JoinTest : public ::testing::Test {
 protected:
  typedef std::vector<int> vi;
  typedef vi::iterator actual_iterator;
  vi u, v, w;

  auto make_join_iterator(std::size_t pos = 0) {
    JoinedIterator<actual_iterator, 3> it (
      {u.begin(), v.begin(), w.begin()},
      {u.end(), v.end(), w.end()},
      pos);
    return it;
  }

  virtual void SetUp() {
    u = {1, 2, 3};
    v = {4, 5, 6};
    w = {7, 8, 9};
  }
};

TEST_F(JoinTest, iterator_increment) {
  auto it = make_join_iterator();
  EXPECT_EQ(1, *it);
  ++it; ++it; ++it;   // 2 3 4
  EXPECT_EQ(4, *it);
  it++; it++; it++;
  EXPECT_EQ(7, *it);
}

TEST_F(JoinTest, iterator_compare) {
  auto i = make_join_iterator();
  auto j = make_join_iterator();
  EXPECT_TRUE(i == j);
  EXPECT_FALSE(i != i);
  EXPECT_FALSE(++i == j);
  EXPECT_TRUE(i != j);
}

TEST_F(JoinTest, begin_end) {
  auto r = make_joined(u, v);
  auto it = r.begin();
  EXPECT_EQ(1, *it);
  for (int i=0; i<6; i++) ++it;
  EXPECT_EQ(it, r.end());
}

TEST_F(JoinTest, for) {
  std::vector<int> result;
  for (int n : make_joined(u, v, w)) {
    result.push_back(n);
  }
  for (int i=1; i<=9; i++) {
    EXPECT_EQ(i, result[i-1]);
  }
}

TEST_F(JoinTest, with_stl) {
  std::vector<int> result;
  auto joined = make_joined(u, v, w);
  std::copy(std::begin(joined), std::end(joined), std::back_inserter(result));
  for (int i=1; i<=9; i++) {
    EXPECT_EQ(i, result[i-1]);
  }
}
