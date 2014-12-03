project(particles)

cmake_minimum_required(VERSION 2.8)

add_definitions("-Wall -std=c++14")

enable_testing()
macro(add_gtest test_name src libs)
  set(test_exec ${test_name}.out)
  add_executable(${test_exec} ${src})
  target_link_libraries(${test_exec} gtest gtest_main pthread ${libs})
  add_test(${test_exec} ${CMAKE_CURRENT_BINARY_DIR}/${test_exec})
endmacro(add_gtest)

include_directories(src)

add_subdirectory(test)
