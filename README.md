particles
=========

# About

Particle system simulation library in C++.

# Build

```bash
mkdir build
cd build
cmake ..
make
make test   # optional
make install
```

# Dependency

- [CGAL](https://www.cgal.org/)
- [googletest](https://code.google.com/p/googletest/) (for testing)

# Minimum example

## example.cpp

```cpp
#include <iostream>
#include <particles/particles.hpp>

int main() {
  particles::Vec<double, 2> v {1, 2};
  std::cout << v << std::endl;
  return 0;
}
```

## CMakeLists.txt

```cmake
project(example)
cmake_minimum_required(VERSION 2.8)

add_definitions("-Wall -std=c++14")

# CGAL and its components
find_package(CGAL)
include( ${CGAL_USE_FILE} )

add_executable(example example.cpp)
```

## build

```sh
mkdir build && cd build
cmake ..
make
```
