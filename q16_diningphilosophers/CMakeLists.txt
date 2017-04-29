cmake_minimum_required(VERSION 3.1.3)
project(q12_diningphilosophers)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})


# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Deadlock ===============================
add_executable(q12_diningphilosophers_deadlock src/dining_deadlock.cpp)

# add required libraries
target_link_libraries(q12_diningphilosophers_deadlock Threads::Threads)

# set output directory and executable name
set_target_properties( q12_diningphilosophers_deadlock PROPERTIES
        OUTPUT_NAME "dining_deadlock")

#==============  Livelock ===============================
add_executable(q12_diningphilosophers_livelock src/dining_livelock.cpp)

# add required libraries
target_link_libraries(q12_diningphilosophers_livelock Threads::Threads)

# set output directory and executable name
set_target_properties( q12_diningphilosophers_livelock PROPERTIES
        OUTPUT_NAME "dining_livelock")


#==============  Timing ===============================
add_executable(q12_diningphilosophers_solution_timing src/dining_solution_timing.cpp)

# add required libraries
target_link_libraries(q12_diningphilosophers_solution_timing Threads::Threads)

# set output directory and executable name
set_target_properties( q12_diningphilosophers_solution_timing PROPERTIES
        OUTPUT_NAME "dining_solution_timing")

#==============  Ordered ===============================
add_executable(q12_diningphilosophers_solution_ordered src/dining_solution_ordered.cpp)

# add required libraries
target_link_libraries(q12_diningphilosophers_solution_ordered Threads::Threads)

# set output directory and executable name
set_target_properties( q12_diningphilosophers_solution_ordered PROPERTIES
        OUTPUT_NAME "dining_solution_ordered")