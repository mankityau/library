cmake_minimum_required(VERSION 3.1.3)
project(q10_timer)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})

# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Timer ===============================
add_executable(q10_timer_timer src/timer.cpp)

# add required libraries
target_link_libraries(q10_timer_timer Threads::Threads)

# set output directory and executable name
set_target_properties( q10_timer_timer PROPERTIES
        OUTPUT_NAME "timer")

# assets
add_custom_command(TARGET q10_timer_timer
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/src/timer.dat"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/timer.dat"
        )