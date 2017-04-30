cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include src)

#==============  Process Child ===============================

add_executable(${PROJECT}_process_child_unsafe src/process/child_unsafe.cpp)
target_include_directories(${PROJECT}_process_child_unsafe PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(${PROJECT}_process_child_unsafe Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_child_unsafe rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_child_unsafe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "child_unsafe")

#==============  Process Child ===============================

add_executable(${PROJECT}_process_child_safe src/process/child_safe.cpp)
target_include_directories(${PROJECT}_process_child_safe PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(${PROJECT}_process_child_safe Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_child_safe rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_child_safe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "child_safe")

#==============  Process Parent ===============================
add_executable(${PROJECT}_process_parent src/process/parent.cpp)
target_include_directories(${PROJECT}_process_parent PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(${PROJECT}_process_parent Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_parent rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_parent PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "parent")

# parent depends on child
add_dependencies(${PROJECT}_process_parent ${PROJECT}_process_child_safe ${PROJECT}_process_child_unsafe)

#==============  Thread ===============================

add_executable(${PROJECT}_thread_thread src/thread/main.cpp)

# add required libraries
target_link_libraries(${PROJECT}_thread_thread Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_thread_thread PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "thread")