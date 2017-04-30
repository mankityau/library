cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Single Pipeline  Child ===============================
add_executable(${PROJECT}_single_child src/single/child.cpp)
target_link_libraries(${PROJECT}_single_child Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_single_child rt)
endif()
set_target_properties(${PROJECT}_single_child PROPERTIES
        OUTPUT_NAME "child"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/single")

#==============  Single Pipeline Parent  ==============================
add_executable(${PROJECT}_single_parent src/single/parent.cpp)
target_link_libraries(${PROJECT}_single_parent Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_single_parent rt)
endif()
set_target_properties(${PROJECT}_single_parent PROPERTIES
        OUTPUT_NAME "parent"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/single")
add_dependencies(${PROJECT}_single_parent ${PROJECT}_single_child)   # parent depends on child

#==============  Multiple Pipeline  Child ===============================
add_executable(${PROJECT}_multiple_child src/multiple/child.cpp)
target_link_libraries(${PROJECT}_multiple_child Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_multiple_child rt)
endif()
set_target_properties(${PROJECT}_multiple_child PROPERTIES
        OUTPUT_NAME "child"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/multiple")

#==============  Multiple Pipeline Parent Poll  ==============================
add_executable(${PROJECT}_multiple_parent_poll src/multiple/parent_poll.cpp)
target_link_libraries(${PROJECT}_multiple_parent_poll Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_multiple_parent_poll rt)
endif()
set_target_properties(${PROJECT}_multiple_parent_poll PROPERTIES
        OUTPUT_NAME "parent_poll"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/multiple")
add_dependencies(${PROJECT}_multiple_parent_poll ${PROJECT}_multiple_child)   # parent depends on child

#==============  Multiple Pipeline Parent Threads  ==============================
add_executable(${PROJECT}_multiple_parent_threads src/multiple/parent_threads.cpp)
target_link_libraries(${PROJECT}_multiple_parent_threads Threads::Threads)    # link with threads library
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_multiple_parent_threads rt)
endif()
set_target_properties(${PROJECT}_multiple_parent_threads PROPERTIES
        OUTPUT_NAME "parent_threads"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/multiple")
add_dependencies(${PROJECT}_multiple_parent_threads ${PROJECT}_multiple_child)   # parent depends on child