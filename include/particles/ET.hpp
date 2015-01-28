/**
 * @file ET.hpp
 *
 * @brief Expression template
 *
 * Concept about Expression
 *
 * operator[]
 */


namespace particles {
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
 * Each operators return this object instead of value itself. As a result,
 * expression will be evaluated at assigning operator.
 */
template <class L, class Op, class R>
struct Exp {
  typedef R value_type;
  const L& l;
  const R& r;

  Exp(const L& l, const R& r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }

  template <class R2>
  auto operator+(const R2& r2) {
    return Exp<Exp<L, Op, R>, Plus, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    return Exp<Exp<L, Op, R>, Minus, R2>(*this, r2);
  }

  template <class T>
  auto operator*(T x) {
    return Exp<Exp<L, Op, R>, Multiply, Scalar<T>>(*this, Scalar<T>(x));
  }

  template <class T>
  auto operator/(T x) {
    return Exp<Exp<L, Op, R>, Divide, Scalar<T>>(*this, Scalar<T>(x));
  }
};


/**
 * @brief copy the right side for scalar
 */
template <class L, class Op, class T>
struct Exp<L, Op, Scalar<T>> {
  typedef T value_type;
  typedef Scalar<T> R;
  const L& l;
  const R r;

  Exp(const L& l, const R& r) : l(l), r(r) {}
  auto operator[](std::size_t i) const { return Op::apply(l[i], r[i]); }

  template <class R2>
  auto operator+(const R2& r2) {
    return Exp<Exp<L, Op, R>, Plus, R2>(*this, r2);
  }

  template <class R2>
  auto operator-(const R2& r2) {
    return Exp<Exp<L, Op, R>, Minus, R2>(*this, r2);
  }

  template <class U>
  auto operator*(const Scalar<U>& s) {
    return Exp<Exp<L, Op, R>, Multiply, Scalar<U>>(*this, s);
  }

  template <class U>
  auto operator/(const Scalar<U>& s) {
    return Exp<Exp<L, Op, R>, Divide, Scalar<U>>(*this, s);
  }
};
}  // namespace ET
}  // namespace particles


namespace std {
template <size_t I, class L, class Op, class R>
inline auto get(const particles::ET::Exp<L, Op, R>& e) {
  return Op::apply(get<I>(e.l), get<I>(e.r));
}
}  // namespace std
