cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})

set(CMAKE_CXX_STANDARD 11)

# customize output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

# directories to search for header files
include_directories(../../include)

#==============  Children  ======================================
add_executable(${PROJECT}_child1 src/child1.cpp)
set_target_properties(${PROJECT}_child1 PROPERTIES OUTPUT_NAME "child1")

add_executable(${PROJECT}_child2 src/child2.cpp)
set_target_properties(${PROJECT}_child2 PROPERTIES OUTPUT_NAME "child2")

add_executable(${PROJECT}_child3 src/child3.cpp)
set_target_properties(${PROJECT}_child3 PROPERTIES OUTPUT_NAME "child3")

#==============  Parent  ======================================
add_executable(${PROJECT}_parent src/parent.cpp)
set_target_properties(${PROJECT}_parent PROPERTIES OUTPUT_NAME "parent")

# Parent executable depends on all children (since it tries to spawn them)
add_dependencies(${PROJECT}_parent ${PROJECT}_child1 ${PROJECT}_child2 ${PROJECT}_child3)

# install all targets
install(TARGETS ${PROJECT}_child1 ${PROJECT}_child2 ${PROJECT}_child3 ${PROJECT}_parent
        DESTINATION bin/${MY_OUTPUT_DIR})
