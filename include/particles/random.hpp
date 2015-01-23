/**
 * @file random.hpp
 * @bried utilities for using random numbers
 *
 * @todo isotoropic rand (on a sphere)
 */

#pragma once

#include "util.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <functional>
#include <random>
#include <type_traits>
#include <vector>

#include <iostream>

namespace particles {
namespace random {
namespace internal {

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
 * @brief set seed to current time
 */
template <class Engine>
void set_seed_now(Engine& engine) {
  auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
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
  DISALLOW_COPY_AND_ASSIGN(RandomGeneratorBase);
};

/**
 * @brief gives type of uniform distribution adequate to the value type
 */
template <class T, bool is_integral = std::is_integral<T>::value,
          bool is_floating_point = std::is_floating_point<T>::value>
struct UniformDistributionType;

template <class T>
struct UniformDistributionType<T, true, false> {
  typedef std::uniform_int_distribution<T> type;
};

template<class T>
struct UniformDistributionType<T, false, true> {
  typedef std::uniform_real_distribution<T> type;
};

}  // namespace internal


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
    : public internal::RandomGeneratorBase<UniformRand<T, Engine>, Engine> {
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
  typename internal::UniformDistributionType<T>::type distribution_;
};

template <class T>
inline T get_rand(T min, T max) {
  UniformRand<T>::set_range(min, max);
  return UniformRand<T>::get();
}

template <class T, std::size_t N, class Engine = std::mt19937>
struct IsotoropicRand;

template <class T, class Engine>
struct IsotoropicRand<T, 2, Engine> {
  struct Expression {
    T x[2];

    Expression(T r) {
      T theta = get_rand<T>(0, static_cast<T>(2 * M_PI));
      x[0] = r * cos(theta);
      x[1] = r * sin(theta);
    }
    T operator[](std::size_t i) const { return x[i]; }
  };
  static Expression get_vec(T r) { return Expression(r); }
};

template <class T, class Engine>
struct IsotoropicRand<T, 3, Engine> {
  struct Expression {
    typedef T value_type;
    T x[3];

    Expression(T r) {
      T phi = get_rand<T>(0, static_cast<T>(2*M_PI));
      T theta = get_rand<T>(0, static_cast<T>(M_PI));
      x[0] = r * sin(theta) * cos(phi);
      x[1] = r * sin(theta) * sin(phi);
      x[2] = r * cos(theta);
    }
    T operator[](std::size_t i) const { return x[i]; }
  };
  static Expression get_vec(T r) { return Expression(r); }
};

template <class Engine>
class GeneratorBase {
 public:
  /** @brief set seed to the current time */
  void seed_now() { internal::set_seed_now(engine_); }
  /** @brief set seed using device */
  void seed_dev() { internal::set_seed_seq(engine_); }
  /** @brief sed seed by your self */
  void seed(typename Engine::result_type val) { engine_.seed(val); }

 protected:
  mutable Engine engine_;
};

/**
 * @brief random number generator of uniform distribution
 *
 * Operations between Vec is available if UniformGenerator is placed at most
 * right position.
 *
 * @code
 * UniformGenerator<int> gen(0, 10);  // 0, ..., 10
 * cout << gen() << endl;
 * Vec<int, 2> v;
 * v = gen;
 * cout << v << endl;
 * @endcode
 */
template <class T, class Engine = std::mt19937>
class UniformGenerator : public GeneratorBase<Engine> {
  typedef GeneratorBase<Engine> Base;
 public:
  typedef T value_type;

  UniformGenerator() : distribution_() {}
  UniformGenerator(T a, T b) : distribution_(a, b) {}

  /** @brief get a random number */
  value_type operator()() const { return distribution_(Base::engine_); }
  /** @brief get a random number */
  value_type operator[](std::size_t) const { return (*this)(); }

 private:
  mutable typename internal::UniformDistributionType<T>::type distribution_;
};


}  // namespace random
}  // namespace particles
