cmake_minimum_required(VERSION 3.1.3)
project(q8_readwrite)

set(CMAKE_CXX_STANDARD 14) # <shared_mutex> for threads requires c++14!!!

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})


# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Thread Reader ===============================
add_executable(q8_readwrite_thread_read src/thread/read_priority.cpp)

# add required libraries
target_link_libraries(q8_readwrite_thread_read Threads::Threads)

# set output directory and executable name
set_target_properties( q8_readwrite_thread_read PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "read_priority")

#==============  Process Reader ===============================
add_executable(q8_readwrite_process_read src/process/read_priority.cpp)

# add required libraries
target_link_libraries(q8_readwrite_process_read Threads::Threads)
if(UNIX)
    target_link_libraries(q8_readwrite_process_read rt)
endif()

# set output directory and executable name
set_target_properties( q8_readwrite_process_read PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "read_priority")


#==============  Thread Writer ===============================
add_executable(q8_readwrite_thread_write src/thread/write_priority.cpp)

# add required libraries
target_link_libraries(q8_readwrite_thread_write Threads::Threads)

# set output directory and executable name
set_target_properties( q8_readwrite_thread_write PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "write_priority")

#==============  Process Writer ===============================
add_executable(q8_readwrite_process_write src/process/write_priority.cpp)

# add required libraries
target_link_libraries(q8_readwrite_process_write Threads::Threads)
if(UNIX)
    target_link_libraries(q8_readwrite_process_write rt)
endif()

# set output directory and executable name
set_target_properties( q8_readwrite_process_write PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "write_priority")

#==============  Thread Fair ===============================
add_executable(q8_readwrite_thread_fair src/thread/fair_priority.cpp)

# add required libraries
target_link_libraries(q8_readwrite_thread_fair Threads::Threads)

# set output directory and executable name
set_target_properties( q8_readwrite_thread_fair PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "fair_priority")

#==============  Process Fair ===============================
add_executable(q8_readwrite_process_fair src/process/fair_priority.cpp)

# add required libraries
target_link_libraries(q8_readwrite_process_fair Threads::Threads)
if(UNIX)
    target_link_libraries(q8_readwrite_process_fair rt)
endif()

# set output directory and executable name
set_target_properties( q8_readwrite_process_fair PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "fair_priority")