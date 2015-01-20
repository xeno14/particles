project(particles)

cmake_minimum_required(VERSION 2.8)

add_definitions("-Wall -std=c++14")


###########################################################
# CGAL
###########################################################
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )  
endif()

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )
  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  
endif()

# include helper file
include( ${CGAL_USE_FILE} )

include( CGAL_CreateSingleSourceCGALProgram )


###########################################################
# testing
###########################################################
enable_testing()
macro(add_gtest test_name src libs)
  set(test_exec ${test_name}.out)
  add_executable(${test_exec} ${src})
  target_link_libraries(${test_exec} gtest gtest_main pthread ${libs})
  add_test(${test_exec} ${CMAKE_CURRENT_BINARY_DIR}/${test_exec})
endmacro(add_gtest)


###########################################################
# subdirectory
###########################################################
include_directories(include)
add_subdirectory(test)
add_subdirectory(example)


###########################################################
# install
###########################################################
install(DIRECTORY include/
   DESTINATION /usr/local/include
   FILES_MATCHING PATTERN "*.hpp")
