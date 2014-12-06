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
 * @brief singleton 
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
 * @todo set seed
 *
 * @code
 *  UniformRand<double>::set_range(-1,1);
 *  auto x = UniformRealRand<double>::get();  // double in [-1, 1)
 *
 *  UniformRand<int<::set_range(0, 100);
 *  auto n = UniformRand<int>::get();         // int in {0, 1, ...,100}
 * @endcode
 */
template <class T, class Engine = std::mt19937>
class UniformRand
    : public RandomGeneratorBase<UniformRand<T, Engine>, Engine> {
 public:
  UniformRand() : distribution_(0, 1) {}

  /**
   * corresponds to expression::Expression
   */
  struct Expression {
    typedef T value_type;
    T operator[](std::size_t) const { return get(); }
  };

  /** 
   * @brief returns a generated random number
   */
  static T get() {
    static auto& o = UniformRand::instance();
    return o.distribution_(o.engine);
  }
  
  /**
   * @brief returns expression available for particles::Vec
   *
   * @code
   * UniformRand<double>::set_range(0, 1);
   * Vec<double, 3> u {1, 2, 3}, v {3, 2, 1}, result;
   *
   * // result = { -2+a, 0+b, 2+c }
   * // a,b,c: generated numbers at different time
   * result = u - v + UniformRand<double>::get_vec();
   * @endcode
   */
  static const Expression& get_vec() { static Expression e; return e; }

  /**
   * @brief set range of random number
   * @pre a <= b
   * @param a min
   * @param b max (for integer, b is included, but for float, it is not)
   */
  static void set_range(T a, T b) {
    static auto& o = UniformRand::instance();
    o.distribution_ = decltype(o.distribution_)(a, b);
  }

 private:
  typename UniformDistribution<T>::type distribution_;
};

}  // namespace random
}  // namespace particles
