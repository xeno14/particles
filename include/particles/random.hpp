/**
 * @file random.hpp
 * @bried utilities for using random numbers
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
 * @deprecated
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

  value_type get() const { return distribution_(Base::engine_); }

  /** @brief get a random number */
  // value_type operator()() const { return distribution_(Base::engine_); }
  const UniformGenerator& operator()() const { return *this; }

  operator value_type() const { return get(); }

  bool operator == (value_type n) const { return get() == n; }
  bool operator != (value_type n) const { return get() != n; }
  bool operator <  (value_type n) const { return get() <  n; }
  bool operator >  (value_type n) const { return get() >  n; }
  bool operator <= (value_type n) const { return get() <= n; }
  bool operator >= (value_type n) const { return get() >= n; }

  /** @brief get a random number */
  value_type operator[](std::size_t) const { return get(); }

 private:
  mutable typename internal::UniformDistributionType<T>::type distribution_;
};

/**
 * @brief get value on N-dimension sphere)
 *
 * - N=2: on S1
 * - N=3: on S2
 *
 * @code
 * UniformOnSphere<double, 2> circle(1.0);
 * Vec<double, 2> v;
 * v = circle();  // get new value
 * v = circle;    // same value as above
 * v = circle();  // get new value
 * @code
 */
template <class T, std::size_t N, class Engine = std::mt19937>
class UniformOnSphere;

template <class T, class Engine>
class UniformOnSphere<T, 2, Engine> : public GeneratorBase<Engine> {
  typedef GeneratorBase<Engine> Base;

 public:
  typedef T value_type;

  UniformOnSphere(T r=1) : r_(r), dist_theta_(0, M_PI*2) {}

  value_type theta() const { return dist_theta_(Base::engine_); }

  /** @brief defines next value of theta */
  UniformOnSphere& operator()() { theta_ = theta(); return *this; }

  value_type operator[](std::size_t i) const {
    typedef std::function<value_type(value_type, value_type)> func_t;
    static func_t funcs[] = {[](value_type r,
                                value_type theta) { return r * cos(theta); },
                             [](value_type r,
                                value_type theta) { return r * sin(theta); }};
    return funcs[i](r_, theta_);
  }

 private:
  value_type theta_;
  const T r_;
  mutable typename internal::UniformDistributionType<T>::type dist_theta_;
};

template <class T, class Engine>
class UniformOnSphere<T, 3, Engine> : public GeneratorBase<Engine> {
  typedef GeneratorBase<Engine> Base;

 public:
  typedef T value_type;

  UniformOnSphere(T r=1) : r_(r), dist_phi_(0, M_PI*2), dist_theta_(0, M_PI) {}

  value_type phi()   const { return dist_phi_(Base::engine_); }
  value_type theta() const { return dist_theta_(Base::engine_); }

  /** @brief defines next value of theta */
  UniformOnSphere& operator()() { phi_ = phi(); theta_ = theta(); return *this; }

  value_type operator[](std::size_t i) const {
    typedef std::function<value_type(value_type, value_type, value_type)> func_t;
    static func_t funcs[] =
        {[](value_type r, value_type phi, value_type theta)
           { return r * sin(theta) * cos(phi); },
         [](value_type r, value_type phi, value_type theta)
           { return r * sin(theta) * sin(phi); },
         [](value_type r, value_type phi, value_type theta)
           { return r * cos(theta); }};
    return funcs[i](r_, phi_, theta_);
  }

 private:
  value_type phi_, theta_;
  const T r_;
  mutable typename internal::UniformDistributionType<T>::type dist_phi_;
  mutable typename internal::UniformDistributionType<T>::type dist_theta_;
};

}  // namespace random
}  // namespace particles
