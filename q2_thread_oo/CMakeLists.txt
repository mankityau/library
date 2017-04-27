cmake_minimum_required(VERSION 3.1.3)
project(q1_thread_oo)

set(CMAKE_CXX_STANDARD 11)

# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories("../include")

#==============  Threads OO  ======================================
add_executable(q2threadoo src/q2threadoo.cpp)
target_link_libraries(q2threadoo Threads::Threads)

#==============  Class with threads ===============================
add_executable(q2classthreads src/q2classthreads.cpp)
target_link_libraries(q2classthreads Threads::Threads)