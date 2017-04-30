cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT q8_condition_variable NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Condition Car ===============================
add_executable(${PROJECT}_process_dishwasher src/process/dishwasher.cpp)
target_link_libraries(${PROJECT}_process_dishwasher Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_dishwasher rt)
endif()
set_target_properties(${PROJECT}_process_dishwasher PROPERTIES
        OUTPUT_NAME "dishwasher"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

#==============  Condition Pedestrian =======================
add_executable(${PROJECT}_process_customer src/process/customer.cpp)
target_link_libraries(${PROJECT}_process_customer Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_process_customer rt)
endif()
set_target_properties(${PROJECT}_process_customer PROPERTIES
        OUTPUT_NAME "customer"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

#==============  Condition Parent  ==============================
add_executable(${PROJECT}_process_restaurant src/process/restaurant.cpp)
target_link_libraries(${PROJECT}_process_restaurant Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_process_restaurant rt)
endif()
set_target_properties(${PROJECT}_process_restaurant PROPERTIES
        OUTPUT_NAME "restaurant"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process")

add_dependencies(${PROJECT}_process_restaurant ${PROJECT}_process_dishwasher ${PROJECT}_process_customer)

#==============  Condition Thread  ==============================
add_executable(${PROJECT}_thread_restaurant src/thread/restaurant.cpp)
target_link_libraries(${PROJECT}_thread_restaurant Threads::Threads)
set_target_properties(${PROJECT}_thread_restaurant PROPERTIES
        OUTPUT_NAME "restaurant"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread")