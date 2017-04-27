cmake_minimum_required(VERSION 3.1.3)
project(labs)

set(CMAKE_CXX_STANDARD 11)

# add cpen333 header library
include_directories("include")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory("q1_multiprocess")
add_subdirectory("q1_multithread")
add_subdirectory("q2_thread_oo")
add_subdirectory("q3_datapools")
add_subdirectory("q6_condition")
add_subdirectory("q6_event")
add_subdirectory("q6_rendezvous")
add_subdirectory("q7_pipeline")
add_subdirectory("q7_multipipe")
add_subdirectory("q8_mutex")
add_subdirectory("q8_readwrite")
add_subdirectory("q8_monitor")
add_subdirectory("q9_producerconsumer")