/**
 * @file random.hpp
 * @bried utilities for using random numbers
 * @todo substitute to vec directly, operator[] and std::size
 */

#pragma once

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <random>
#include <type_traits>
#include <vector>

namespace particles {
namespace random {
namespace {

/**
 * @biref set sequence of randoms generated at random_devide
 * @tparam Engine
 * @param n size of sequence of seeds
 */
template <class Engine>
void set_seed_seq(Engine& engine, std::size_t n = 10) {
  std::random_device rd;
  std::vector<std::uint_least32_t> v(n);
  std::generate(v.begin(), v.end(), std::ref(rd));
  std::seed_seq seed(v.begin(), v.end());
  engine.seed(seed);
}

/**
 *
 * @tparam Subclass type of subclass
 * @tparam Engine engine
 */
template <class SubClass, class Engine>
class RandomGeneratorBase {
 protected:
  Engine engine;

  RandomGeneratorBase() : engine() { set_seed_seq(engine); }
  virtual ~RandomGeneratorBase() {}

  static SubClass& instance() {
    static SubClass o;
    return o;
  }

 private:
  RandomGeneratorBase(const RandomGeneratorBase&) = delete;
  RandomGeneratorBase& operator=(const RandomGeneratorBase&) = delete;
};

/**
 * @brief gives type of uniform distribution adequate to the value type
 */
template <class T, bool is_integral = std::is_integral<T>::value,
          bool is_floating_point = std::is_floating_point<T>::value>
struct UniformDistribution;

template <class T>
struct UniformDistribution<T, true, false> {
  typedef std::uniform_int_distribution<T> type;
};

template<class T>
struct UniformDistribution<T, false, true> {
  typedef std::uniform_real_distribution<T> type;
};

}  // namespace


/**
 * @brief generates random numbers in a certain range
 * @tparam T real type
 * @tparam Engine engine
 * @example
 *  UniformRealRand<double>::set_range(-1,1);
 *  auto x = UniformRealRand<double>::get();
 */
template <class T, class Engine = std::mt19937>
class UniformRand
    : public RandomGeneratorBase<UniformRand<T, Engine>, Engine> {
 public:
  UniformRand() : distribution_(0, 1) {}
  static T get(T min = 0, T max = 1) {
    static auto& o = UniformRand::instance();
    return o.distribution_(o.engine);
  }
  static void set_range(T a, T b) {
    static auto& o = UniformRand::instance();
    o.distribution_ = decltype(o.distribution_)(a, b);
  }

 private:
  typename UniformDistribution<T>::type distribution_;
};

}  // namespace random
}  // namespace particles
