#==========================================================
#  CMAKE Setup
#==========================================================

cmake_minimum_required(VERSION 3.1.3)
project(labs)

# customize binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # separate binaries from build files

# set default install prefix to source directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "default install path" FORCE )
endif()

set(MY_OUTPUT_DIR "")      # blank output directory

# include Doxygen documentation support
include(Doxygen.cmake)

# custom macros
include(Macros.cmake)
warnall()  # enable all warnings

#==========================================================
#  Actual Labs
#==========================================================

# Introduction to C++ and Concurrency
add_subdirectory("q0_basics")
add_subdirectory("q1_threads")
add_subdirectory("q2_processes")

# Inter-Process Communication
add_subdirectory("q3_datapools")
add_subdirectory("q4_pipes")
add_subdirectory("q5_message_queues")

# Synchronization
add_subdirectory("q6_mutex")
add_subdirectory("q7_semaphore")
add_subdirectory("q8_condition")
add_subdirectory("q9_condition_variable")
add_subdirectory("q10_event")
add_subdirectory("q11_timer")
add_subdirectory("q12_rendezvous")
add_subdirectory("q13_monitor")

# Advanced Synchronization Problems
add_subdirectory("q14_reader_writer")
add_subdirectory("q15_producer_consumer")
add_subdirectory("q16_sleeping_barber")
add_subdirectory("q17_dining_philosophers")

# Tools
add_subdirectory("tools")


#==========================================================
#  Testing and Install
#==========================================================

# Tests
# Enable top-level testing
# include (CTest)
enable_testing()
add_subdirectory("test")

# install header library if not current install directory
if (NOT ${CMAKE_INSTALL_PREFIX} EQUAL ${CMAKE_SOURCE_DIR} )
    install(DIRECTORY include/cpen333 DESTINATION include/)
endif()