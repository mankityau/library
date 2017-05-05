cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

include(CTest) 

enable_testing()

#==============  Unlinker  ====================================

add_executable(${PROJECT}_process_unlinker_unlinker src/process/unlinker/unlinker.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_unlinker_unlinker Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_process_unlinker_unlinker PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process/unlinker"
        OUTPUT_NAME "unlinker")

add_test(NAME ${PROJECT}_process_unlinker_unlinker_test COMMAND ${PROJECT}_process_unlinker_unlinker)
