cmake_minimum_required(VERSION 3.1.3)
project(q8_monitor)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})


# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Process Child ===============================
add_executable(q8_monitor_child src/child.cpp)

# add required libraries
target_link_libraries(q8_monitor_child Threads::Threads)
if(UNIX)
    target_link_libraries(q8_monitor_child rt)
endif()

# set output directory and executable name
set_target_properties( q8_monitor_child PROPERTIES
        OUTPUT_NAME "child")

#==============  Process Parent ===============================

add_executable(q8_monitor_mom src/mom.cpp)

# add required libraries
target_link_libraries(q8_monitor_mom Threads::Threads)
if(UNIX)
    target_link_libraries(q8_monitor_mom rt)
endif()

# set output directory and executable name
set_target_properties( q8_monitor_mom PROPERTIES
        OUTPUT_NAME "mom")

# mom depends on child
add_dependencies(q8_monitor_mom q8_monitor_child)

