cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Process Child ===============================
add_executable(${PROJECT}_process_child src/process/child.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_child Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_child rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_child PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "child")

#==============  Process Parent ===============================

add_executable(${PROJECT}_process_mom src/process/mom.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_mom Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_mom rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_mom PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "mom")

# mom depends on child
add_dependencies(${PROJECT}_process_mom ${PROJECT}_thread_spender)

#==============  Process Child ===============================
add_executable(${PROJECT}_thread_spender src/thread/spender.cpp)

# add required libraries
target_link_libraries(${PROJECT}_thread_spender Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_thread_spender rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_thread_spender PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "spender")

