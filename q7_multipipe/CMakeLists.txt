cmake_minimum_required(VERSION 3.1.3)
project(q7_multipipe)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})


# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Process Child ===============================
add_executable(q7_multipipe_child src/child.cpp)

# add required libraries
target_link_libraries(q7_multipipe_child Threads::Threads)
if(UNIX)
    target_link_libraries(q7_multipipe_child rt)
endif()

# set output directory and executable name
set_target_properties( q7_multipipe_child PROPERTIES
        OUTPUT_NAME "child")

#==============  Process Parent Polling ===============================

add_executable(q7_multipipe_parent_poll src/parent_poll.cpp)

# add required libraries
target_link_libraries(q7_multipipe_parent_poll Threads::Threads)
if(UNIX)
    target_link_libraries(q7_multipipe_parent_poll rt)
endif()

# set output directory and executable name
set_target_properties( q7_multipipe_parent_poll PROPERTIES
        OUTPUT_NAME "parent_poll")

# factory depends on both robot and production_line
add_dependencies(q7_multipipe_parent_poll q7_multipipe_child)

#==============  Process Parent Threads ===============================

add_executable(q7_multipipe_parent_threads src/parent_threads.cpp)

# add required libraries
target_link_libraries(q7_multipipe_parent_threads Threads::Threads)
if(UNIX)
    target_link_libraries(q7_multipipe_parent_threads rt)
endif()

# set output directory and executable name
set_target_properties( q7_multipipe_parent_threads PROPERTIES
        OUTPUT_NAME "parent_threads")

# factory depends on both robot and production_line
add_dependencies(q7_multipipe_parent_threads q7_multipipe_child)