cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Deadlock ===============================
add_executable(${PROJECT}_dining_deadlock src/dining_deadlock.cpp)

# add required libraries
target_link_libraries(${PROJECT}_dining_deadlock Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_dining_deadlock PROPERTIES
        OUTPUT_NAME "dining_deadlock")

#==============  Livelock ===============================
add_executable(${PROJECT}_dining_livelock src/dining_livelock.cpp)

# add required libraries
target_link_libraries(${PROJECT}_dining_livelock Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_dining_livelock PROPERTIES
        OUTPUT_NAME "dining_livelock")


#==============  Timing ===============================
add_executable(${PROJECT}_solution_timing src/dining_solution_timing.cpp)

# add required libraries
target_link_libraries(${PROJECT}_solution_timing Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_solution_timing PROPERTIES
        OUTPUT_NAME "dining_solution_timing")

#==============  Ordered ===============================
add_executable(${PROJECT}_solution_ordered src/dining_solution_ordered.cpp)

# add required libraries
target_link_libraries(${PROJECT}_solution_ordered Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_solution_ordered PROPERTIES
        OUTPUT_NAME "dining_solution_ordered")