cmake_minimum_required(VERSION 3.1.3)
project(q6_event)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})

# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Process Robot ===============================
add_executable(q6_event_process_robot src/process/robot.cpp)
target_include_directories(q6_event_process_robot PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(q6_event_process_robot Threads::Threads)
if(UNIX)
    target_link_libraries(q6_event_process_robot rt)
endif()

# set output directory and executable name
set_target_properties( q6_event_process_robot PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "robot")

#==============  Process Production Line ===============================

add_executable(q6_event_process_production_line src/process/production_line.cpp)
target_include_directories(q6_event_process_production_line PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(q6_event_process_production_line Threads::Threads)
if(UNIX)
    target_link_libraries(q6_event_process_production_line rt)
endif()

# set output directory and executable name
set_target_properties( q6_event_process_production_line PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "production_line")

#==============  Process Factory ===============================

add_executable(q6_event_process_factory src/process/factory.cpp)

# add required libraries
target_link_libraries(q6_event_process_factory Threads::Threads)
if(UNIX)
    target_link_libraries(q6_event_process_factory rt)
endif()

# set output directory and executable name
set_target_properties( q6_event_process_factory PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "factory")

# factory depends on both robot and production_line
add_dependencies(q6_event_process_factory q6_event_process_robot q6_event_process_production_line)

#==============  Thread Factory ===============================

add_executable(q6_event_thread_factory src/thread/factory.cpp)

# add required libraries
target_link_libraries(q6_event_thread_factory Threads::Threads)

# set output directory and executable name
set_target_properties( q6_event_thread_factory PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "factory")