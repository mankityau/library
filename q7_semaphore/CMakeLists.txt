cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include src)

#==============  Process Child ===============================

add_executable(${PROJECT}_process_car src/process/car.cpp)
target_include_directories(${PROJECT}_process_car PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(${PROJECT}_process_car Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_car rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_car PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "car")

#==============  Process Parent ===============================
add_executable(${PROJECT}_process_gas_station src/process/gas_station.cpp)
target_include_directories(${PROJECT}_process_gas_station PRIVATE src/process)  # local headers

# add required libraries
target_link_libraries(${PROJECT}_process_gas_station Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_gas_station rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_gas_station PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "gas_station")

# gas_station depends on car
add_dependencies(${PROJECT}_process_gas_station ${PROJECT}_process_car )

#==============  Thread ===============================

add_executable(${PROJECT}_thread_gas_station src/thread/gas_station.cpp)

# add required libraries
target_link_libraries(${PROJECT}_thread_gas_station Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_thread_gas_station PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "gas_station")