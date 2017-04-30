cmake_minimum_required(VERSION 3.1.3)
project(labs)

# customize binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Warnings as errors
if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# Introduction to C++ and Concurrency
add_subdirectory("q0_basics")
add_subdirectory("q1_threads")
add_subdirectory("q2_processes")

# Inter-Process Communication
add_subdirectory("q3_datapools")
add_subdirectory("q4_pipelines")
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
