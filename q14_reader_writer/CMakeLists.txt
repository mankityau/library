cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 14)        # We need C++14 for shared_mutex!!
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Thread Reader ===============================
add_executable(${PROJECT}_thread_read src/thread/read_priority.cpp)

# add required libraries
target_link_libraries(${PROJECT}_thread_read Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_thread_read PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "read_priority")

#==============  Process Reader ===============================
add_executable(${PROJECT}_process_read src/process/read_priority.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_read Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_read rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_read PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "read_priority")


#==============  Thread Writer ===============================
add_executable(${PROJECT}_thread_write src/thread/write_priority.cpp)

# add required libraries
target_link_libraries(${PROJECT}_thread_write Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_thread_write PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "write_priority")

#==============  Process Writer ===============================
add_executable(${PROJECT}_process_write src/process/write_priority.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_write Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_write rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_write PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "write_priority")

#==============  Thread Fair ===============================
add_executable(${PROJECT}_thread_fair src/thread/fair_priority.cpp)

# add required libraries
target_link_libraries(${PROJECT}_thread_fair Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_thread_fair PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "fair_priority")

#==============  Process Fair ===============================
add_executable(${PROJECT}_process_fair src/process/fair_priority.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_fair Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_fair rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_fair PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "fair_priority")