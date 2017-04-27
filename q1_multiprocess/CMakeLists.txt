cmake_minimum_required(VERSION 3.1.3)
project(q1_multiprocess)

set(CMAKE_CXX_STANDARD 11)

# Multithreading
# find_package( Threads REQUIRED )

# library directories
include_directories("../include")

#==============  Child 1  ======================================
add_executable(q1child1 src/q1child1.cpp)

add_executable(q1child2 src/q1child2.cpp)

add_executable(q1child3 src/q1child3.cpp)

#==============  Parent 1  ======================================
add_executable(q1parent src/q1parent.cpp)
add_dependencies(q1parent q1child1 q1child2 q1child3)