cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include src)

#==============  Timer ===============================
add_executable(${PROJECT}_timer src/timer.cpp)

# add required libraries
target_link_libraries(${PROJECT}_timer Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_timer PROPERTIES
        OUTPUT_NAME "timer")

# assets
add_custom_command(TARGET ${PROJECT}_timer
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/src/timer.dat"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/timer.dat"
        )