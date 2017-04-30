cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Datapool Child ===============================
add_executable(${PROJECT}_process_child src/process/child.cpp)
target_link_libraries(${PROJECT}_process_child Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT}_process_child rt)
endif()
set_target_properties(${PROJECT}_process_child PROPERTIES
        OUTPUT_NAME "child"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

#==============  Datapool Parent  ======================================
add_executable(${PROJECT}_process_parent src/process/parent.cpp)
target_link_libraries(${PROJECT}_process_parent Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT}_process_parent rt)
endif()
set_target_properties(${PROJECT}_process_parent PROPERTIES
        OUTPUT_NAME "parent"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")
add_dependencies(${PROJECT}_process_parent ${PROJECT}_process_child) # parent depends on child

#==============  Datapool thread  ======================================
add_executable(${PROJECT}_thread_datapool src/thread/datapool.cpp)
target_link_libraries(${PROJECT}_thread_datapool Threads::Threads)    # link with threads library
set_target_properties(${PROJECT}_thread_datapool PROPERTIES
        OUTPUT_NAME "datapool"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread")
