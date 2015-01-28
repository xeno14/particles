/**
 * @file ET.hpp
 *
 * @brief Expression template
 *
 */


namespace particles {

/**
 * @brief Expression Template
 *
 * Consepts of expression template class:
 *  - has operator[]
 */
namespace ET {

// Operators
struct Plus {
  template <class L, class R>
  static auto apply(L l, R r) { return l + r; }
};

struct Minus {
  template <class L, class R>
  static auto apply(L l, R r) { return l - r; }
};

struct Multiply {
  template <class L, class R>
  static auto apply(L l, R r) { return l * r; }
};

struct Divide {
  template <class L, class R>
  static auto apply(L l, R r) { return l / r; }
};

/**
 * Scalar
 * 
 * Used for multiply and division.
 */
template <class T>
struct Scalar {
  T value;
  Scalar(T x) : value(x) {}
  Scalar(const Scalar<T>& s) : value(s.value) {}
  T operator[](std::size_t i) const { return value; }
};


/**
 * @brief Expression!
 *
 * Operations return Exp class object instead of value itself in order to avoid
 * creating temporary objects. Expression will be evaluated when operator[] is
 * called.
 */
template <class L, class Op, class R>
struct Exp {
  typedef R value_type;
  typedef Exp<L, Op, R> THIS;

  const L& l;
  const R& r;

  Exp(const L& l, const R& r) : l(l), r(r) {}

  /** @brief accessor and apply operation. */
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }

  template <class R2>
  auto operator+(const R2& r2) {
    return Exp<THIS, Plus, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    return Exp<THIS, Minus, R2>(*this, r2);
  }

  template <class T>
  auto operator*(T x) {
    return Exp<THIS, Multiply, Scalar<T>>(*this, Scalar<T>(x));
  }

  template <class T>
  auto operator/(T x) {
    return Exp<THIS, Divide, Scalar<T>>(*this, Scalar<T>(x));
  }
};


/**
 * @brief Partial specialization for Scalar type
 *
 * Copy the right side for scalar.
 */
template <class L, class Op, class T>
struct Exp<L, Op, Scalar<T>> {
  typedef T value_type;
  typedef Scalar<T> R;
  typedef Exp<L, Op, R> THIS;

  const L& l;
  const R r;

  Exp(const L& l, const R& r) : l(l), r(r) {}
  Exp(const L& l, T r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }

  template <class R2>
  auto operator+(const R2& r2) {
    return Exp<THIS, Plus, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    return Exp<THIS, Minus, R2>(*this, r2);
  }

  template <class U>
  auto operator*(const Scalar<U>& s) {
    return Exp<THIS, Multiply, Scalar<U>>(*this, s);
  }

  template <class U>
  auto operator/(const Scalar<U>& s) {
    return Exp<THIS, Divide, Scalar<U>>(*this, s);
  }
};

}  // namespace ET
}  // namespace particles


namespace std {
using namespace particles;

template <size_t I, class L, class Op, class R>
inline auto get(const ET::Exp<L, Op, R>& e) {
  return Op::apply(get<I>(e.l), get<I>(e.r));
}
}  // namespace std
