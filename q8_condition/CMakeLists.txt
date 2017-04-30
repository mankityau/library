cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Condition Car ===============================
add_executable(${PROJECT}_process_car src/process/car.cpp)
target_link_libraries(${PROJECT}_process_car Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_car rt)
endif()
set_target_properties(${PROJECT}_process_car PROPERTIES
        OUTPUT_NAME "car"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

#==============  Condition Pedestrian =======================
add_executable(${PROJECT}_process_pedestrian src/process/pedestrian.cpp)
target_link_libraries(${PROJECT}_process_pedestrian Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_pedestrian rt)
endif()
set_target_properties(${PROJECT}_process_pedestrian PROPERTIES
        OUTPUT_NAME "pedestrian"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

#==============  Condition Parent  ==============================
add_executable(${PROJECT}_process_intersection src/process/intersection.cpp)
target_link_libraries(${PROJECT}_process_intersection Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_process_intersection rt)
endif()
set_target_properties(${PROJECT}_process_intersection PROPERTIES
        OUTPUT_NAME "intersection"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

add_dependencies(${PROJECT}_process_intersection ${PROJECT}_process_car ${PROJECT}_process_pedestrian)

#==============  Condition Thread  ==============================
add_executable(${PROJECT}_thread_intersection src/thread/intersection.cpp)
target_link_libraries(${PROJECT}_thread_intersection Threads::Threads)
set_target_properties(${PROJECT}_thread_intersection PROPERTIES
        OUTPUT_NAME "intersection"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread")