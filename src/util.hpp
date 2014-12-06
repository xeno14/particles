/**
 * @file util.hpp
 *
 * @brief utilities: macros, and others
 */


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
