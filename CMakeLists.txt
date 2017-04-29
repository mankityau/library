cmake_minimum_required(VERSION 3.1.3)
project(labs)

# customize binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Introduction to C++ and Concurrency
add_subdirectory("q0_basics")
add_subdirectory("q1_threads")
add_subdirectory("q2_processes")

# Inter-Process Communication
add_subdirectory("q3_datapools")
add_subdirectory("q4_pipelines")
add_subdirectory("q5_messagequeues")
add_subdirectory("q6_timer")

# Synchronization
add_subdirectory("q7_mutex")
# add_subdirectory("q8_semaphore")
add_subdirectory("q9_condition")
add_subdirectory("q10_event")
add_subdirectory("q11_rendezvous")
add_subdirectory("q12_monitor")

# Advanced Synchronization Problems
add_subdirectory("q13_readerwriter")
add_subdirectory("q14_producerconsumer")
add_subdirectory("q15_sleepingbarber")
add_subdirectory("q16_diningphilosophers")