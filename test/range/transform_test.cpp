#include <gtest/gtest.h>

#include "particles/range/transform.hpp"

using namespace particles;
using namespace particles::range;


class TransformIteratorTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    v = {{1, 2}, {3, 4}, {5, 6}};  
  }
  std::vector<std::pair<int, int>> v;
};

TEST_F(TransformIteratorTest, increments) {
  auto it = transform_iterator(v.begin(), [](auto& p) { return p.first; });
  EXPECT_EQ(1, *it); it++;
  EXPECT_EQ(3, *it++);
  EXPECT_EQ(5, *it);
}

TEST_F(TransformIteratorTest, compare) {
  auto it1 = transform_iterator(v.begin(), [](auto& p) { return p.first; });
  auto it2 = transform_iterator(v.begin(), [](auto&) { return 0; });
  auto it3 = transform_iterator(v.end(),   [](auto&) { return 0; });

  EXPECT_TRUE (it1 == it2);
  EXPECT_FALSE(it1 != it2);
  EXPECT_FALSE(it1 == it3);
  EXPECT_TRUE (it1 != it3);
  EXPECT_FALSE(it2 == it3);
  EXPECT_TRUE (it2 != it3);
}

TEST_F(TransformIteratorTest, reference) {
  auto it1 = transform_iterator(v.begin(),
                              [](auto& p) { return std::ref(p.first); });
  (*it1)--;
  EXPECT_EQ(0, v.begin()->first);
  EXPECT_EQ(2, v.begin()->second);
}

TEST_F(TransformIteratorTest, copy) {
  std::vector<int> result(3);
  auto iter =
      transform_iterator(v.begin(), v.end(), [](auto& p) {return p.first;});
  std::copy(iter.first, iter.second, result.begin());
  EXPECT_EQ(1, result[0]);
  EXPECT_EQ(3, result[1]);
  EXPECT_EQ(5, result[2]);
}
