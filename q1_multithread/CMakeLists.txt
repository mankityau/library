cmake_minimum_required(VERSION 3.1.3)
project(q1_multithread)

set(CMAKE_CXX_STANDARD 11)

# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories("../include")

#==============  Threads 1  ======================================
add_executable(q1threads1 src/q1threads1.cpp)
target_link_libraries(q1threads1 Threads::Threads)

#==============  Threads 2  ======================================
add_executable(q1threads2 src/q1threads2.cpp)
target_link_libraries(q1threads2 Threads::Threads)