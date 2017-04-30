cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include src)

#==============  Simple Timer ===============================
add_executable(${PROJECT}_simple_timer src/simple/timer.cpp)

# add required libraries
target_link_libraries(${PROJECT}_simple_timer Threads::Threads)

# set output directory and executable name
set_target_properties( ${PROJECT}_simple_timer PROPERTIES
        OUTPUT_NAME "timer"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simple")

# assets
set(TIMER_ASSET_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/simple/timer.dat")
set(TIMER_ASSET_DST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simple/timer.dat")
add_custom_command(TARGET ${PROJECT}_simple_timer
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TIMER_ASSET_SRC} ${TIMER_ASSET_DST})

#==============  Process Faucet ===============================
add_executable(${PROJECT}_process_faucet src/process/faucet.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_faucet Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_faucet rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_faucet PROPERTIES
        OUTPUT_NAME "faucet"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

#==============  Process House ===============================
add_executable(${PROJECT}_process_house src/process/house.cpp)

# add required libraries
target_link_libraries(${PROJECT}_process_house Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_house rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_house PROPERTIES
        OUTPUT_NAME "house"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

add_dependencies(${PROJECT}_process_house ${PROJECT}_process_faucet)


#==============  Thread House  ==============================
add_executable(${PROJECT}_thread_house src/thread/house.cpp)
target_link_libraries(${PROJECT}_thread_house Threads::Threads)
set_target_properties(${PROJECT}_thread_house PROPERTIES
        OUTPUT_NAME "house"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread")