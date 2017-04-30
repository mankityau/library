cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Datapool Child ===============================
add_executable(${PROJECT}_child src/process/child.cpp)
target_link_libraries(${PROJECT}_child Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_child rt)
endif()
set_target_properties(${PROJECT}_child PROPERTIES OUTPUT_NAME "child")

#==============  Datapool Parent  ======================================
add_executable(${PROJECT}_parent src/process/parent.cpp)
target_link_libraries(${PROJECT}_parent Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_parent rt)
endif()
set_target_properties(${PROJECT}_parent PROPERTIES
        OUTPUT_NAME "parent"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")
add_dependencies(${PROJECT}_parent ${PROJECT}_child)   # parent depends on child

#==============  Datapool thread  ======================================
add_executable(${PROJECT}_threads src/thread/main.cpp)
target_link_libraries(${PROJECT}_threads Threads::Threads)    # link with threads library
set_target_properties(${PROJECT}_parent PROPERTIES
        OUTPUT_NAME "datapool"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread")