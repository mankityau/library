cmake_minimum_required(VERSION 3.1.3)
project(q8_mutex)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})

# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Process Child ===============================

add_executable(q8_mutex_process_child_unsafe src/process/child_unsafe.cpp)
target_include_directories(q8_mutex_process_child_unsafe PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(q8_mutex_process_child_unsafe Threads::Threads)
if(UNIX)
    target_link_libraries(q8_mutex_process_child_unsafe rt)
endif()

# set output directory and executable name
set_target_properties( q8_mutex_process_child_unsafe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "child_unsafe")

#==============  Process Child ===============================

add_executable(q8_mutex_process_child_safe src/process/child_safe.cpp)
target_include_directories(q8_mutex_process_child_safe PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(q8_mutex_process_child_safe Threads::Threads)
if(UNIX)
    target_link_libraries(q8_mutex_process_child_safe rt)
endif()

# set output directory and executable name
set_target_properties( q8_mutex_process_child_safe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "child_safe")

#==============  Process Parent ===============================
add_executable(q8_mutex_process_parent src/process/parent.cpp)
target_include_directories(q8_mutex_process_parent PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(q8_mutex_process_parent Threads::Threads)
if(UNIX)
    target_link_libraries(q8_mutex_process_parent rt)
endif()

# set output directory and executable name
set_target_properties( q8_mutex_process_parent PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "parent")

# parent depends on child
add_dependencies(q8_mutex_process_parent q8_mutex_process_child_safe q8_mutex_process_child_unsafe)

#==============  Thread ===============================

add_executable(q8_mutex_thread src/thread/thread.cpp)

# add required libraries
target_link_libraries(q8_mutex_thread Threads::Threads)

# set output directory and executable name
set_target_properties( q8_mutex_thread PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "thread")