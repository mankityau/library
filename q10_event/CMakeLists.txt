cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Process Robot ===============================
add_executable(${PROJECT}_process_robot src/process/robot.cpp)
target_include_directories(${PROJECT}_process_robot PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(${PROJECT}_process_robot Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_robot rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_robot PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "robot")

#==============  Process Production Line ===============================

add_executable(${PROJECT}_process_production_line src/process/production_line.cpp)
target_include_directories(${PROJECT}_process_production_line PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(${PROJECT}_process_production_line Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_production_line rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_production_line PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "production_line")

#==============  Process Factory ===============================

add_executable(${PROJECT}_process_factory src/process/factory.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_factory Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_factory rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_factory PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "factory")

# factory depends on both robot and production_line
add_dependencies(${PROJECT}_process_factory ${PROJECT}_process_robot ${PROJECT}_process_production_line)

#==============  Thread Factory ===============================

add_executable(${PROJECT}_thread_factory src/thread/factory.cpp)

# add required libraries
target_link_libraries(${PROJECT}_thread_factory Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_thread_factory PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "factory")