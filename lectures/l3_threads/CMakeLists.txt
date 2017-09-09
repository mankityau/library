cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})

# customize output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR}/${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${MY_OUTPUT_DIR})

set(CMAKE_CXX_STANDARD 11)
include(../../config/Macros.cmake)

# directories to search for header files
include_directories(../../include)

#==============  Lecture Examples ===============================
add_thread_executable(${PROJECT}_windows_threads windows_threads . src/windows_threads.cpp)
add_thread_executable(${PROJECT}_posix_threads posix_threads . src/posix_threads.cpp)
add_thread_executable(${PROJECT}_standard_threads standard_threads . src/standard_threads.cpp)
add_thread_executable(${PROJECT}_shared_memory shared_memory . src/shared_memory.cpp)
add_thread_executable(${PROJECT}_message_passing message_passing . src/message_passing.cpp)
