cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  SEM UNLINK ===============================
add_executable(${PROJECT}_sem_unlink src/sem_unlink.cpp)

# set output directory and executable name
set_target_properties( ${PROJECT}_sem_unlink PROPERTIES
        OUTPUT_NAME "sem_unlink")

#==============  SHM UNLINK ===============================
add_executable(${PROJECT}_shm_unlink src/shm_unlink.cpp)
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT}_shm_unlink rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_shm_unlink PROPERTIES
        OUTPUT_NAME "shm_unlink")
