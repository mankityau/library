cmake_minimum_required(VERSION 3.1.3)
project(q7_pipeline)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})


# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Process Child ===============================
add_executable(q7_pipeline_child src/child.cpp)

# add required libraries
target_link_libraries(q7_pipeline_child Threads::Threads)
if(UNIX)
    target_link_libraries(q7_pipeline_child rt)
endif()

# set output directory and executable name
set_target_properties( q7_pipeline_child PROPERTIES
        OUTPUT_NAME "child")

#==============  Process Parent ===============================

add_executable(q7_pipeline_parent src/parent.cpp)

# add required libraries
target_link_libraries(q7_pipeline_parent Threads::Threads)
if(UNIX)
    target_link_libraries(q7_pipeline_parent rt)
endif()

# set output directory and executable name
set_target_properties( q7_pipeline_parent PROPERTIES
        OUTPUT_NAME "parent")

# factory depends on both robot and production_line
add_dependencies(q7_pipeline_parent q7_pipeline_child)

