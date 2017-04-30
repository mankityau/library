cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include src)

#==============  PenPal ===============================
add_executable(${PROJECT}_penpal src/penpal.cpp)

# add required libraries
target_link_libraries(${PROJECT}_penpal Threads::Threads)
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT}_penpal rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_penpal PROPERTIES
        OUTPUT_NAME "PenPal")

#==============  Main ===============================
add_executable(${PROJECT}_parent src/parent.cpp)

# add required libraries
target_link_libraries(${PROJECT}_parent Threads::Threads)
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT}_parent rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_parent PROPERTIES
        OUTPUT_NAME "parent")

add_dependencies(${PROJECT}_parent ${PROJECT}_penpal)
