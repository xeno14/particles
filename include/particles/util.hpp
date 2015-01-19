#pragma once

/**
 * @file util.hpp
 *
 * @brief utilities: macros, and others
 */

#pragma once

#include <iostream>

/**
 * @brief disallow copy constructor and operator=
 * Use this in private section!
 *
 * @code
 * class Foo {
 *  public:
 *   Foo(int f);
 *   ~Foo();
 *  private:
 *   DISALLOW_COPY_AND_ASSIGN(Foo);
 * };
 * @endcode
 */
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

/**
 * @brief overloads std::begin and std::end
 */
#define OVERLOAD_STD_BEGIN_AND_END(TEMPLATE_PARAM, CLASS) \
  namespace std { \
  template <TEMPLATE_PARAM> \
  inline auto begin(CLASS& c) { \
    return c.begin(); \
  } \
  template <TEMPLATE_PARAM> \
  inline auto end(CLASS& c) { \
    return c.end(); \
  } \
  }

/**
 * @brief check condition
 *
 * @code
 * CHECK(1 == 1) << "one is one\n";     // output nothing
 * CHECK(1 == 2) << "one is two\n";     // terminated here!!
 * @endcode
 */
#define CHECK(cond)                                                       \
  ((cond) ? (particles::util::internal::CheckImpl(true, std::cerr) << "") \
          : (particles::util::internal::CheckImpl(false, std::cerr)       \
             << __FILE__ << ":" << __LINE__ << " " #cond " "))

namespace particles {
namespace util {
namespace internal {

/** @brief output nothing */
class NullOstream : public std::ostream {
  public:
    template <class T>
    std::ostream& operator<<(const T& t) {
      return *this;
    }
};

class CheckImpl {
 public:
  CheckImpl(bool cond, std::ostream& ost) : cond_(cond), ost_(ost) {}
  ~CheckImpl() {
    if (!cond_) std::exit(EXIT_FAILURE);
  }
  template <class T>
  std::ostream& operator<<(const T& t) {
    static NullOstream null_ostream;
    return cond_ ? null_ostream : (ost_ << t);
  }
 private:
 bool cond_;
  std::ostream& ost_;
};

template <class T>
struct TypeFromRef {
  typedef T type;
};

}  // namespace internal

template <class T>
auto type_from_ref(const T& r) {
  return internal::TypeFromRef<T>();
}


}  // namespace util
}  // namespace particles

/**
 * @brief Get type removing reference
 *
 * @code
 * vector<int> v = {1, 2, 3};
 * TYPE_FROM_REF(v.begin()) n = 0;  // equivalent to int n
 * @endcode
 */
#define TYPE_FROM_REF(ref) \
  typename decltype(particles::util::type_from_ref(ref))::type
