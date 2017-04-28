cmake_minimum_required(VERSION 3.1.3)
project(q10_messagequeues)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})


# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  PenPal ===============================
add_executable(q10_messagequeues_penpal src/PenPal.cpp)

# add required libraries
target_link_libraries(q10_messagequeues_penpal Threads::Threads)
if(UNIX)
    target_link_libraries(q10_messagequeues_penpal rt)
endif()

# set output directory and executable name
set_target_properties( q10_messagequeues_penpal PROPERTIES
        OUTPUT_NAME "PenPal")

#==============  Main ===============================
add_executable(q10_messagequeues_main src/main.cpp)

# add required libraries
target_link_libraries(q10_messagequeues_main Threads::Threads)
if(UNIX)
    target_link_libraries(q10_messagequeues_main rt)
endif()

# set output directory and executable name
set_target_properties( q10_messagequeues_main PROPERTIES
        OUTPUT_NAME "main")

add_dependencies(q10_messagequeues_main q10_messagequeues_penpal)