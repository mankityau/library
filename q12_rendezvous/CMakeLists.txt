cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Rendezvous Person ===============================
add_executable(${PROJECT}_process_person src/process/person.cpp)
target_link_libraries(${PROJECT}_process_person Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_process_person rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_person PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "person")

#==============  Rendezvous Roller Coaster ===============================
add_executable(${PROJECT}_process_roller_coaster src/process/roller_coaster.cpp)
target_link_libraries(${PROJECT}_process_roller_coaster Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_process_roller_coaster rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_process_roller_coaster PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/process"
        OUTPUT_NAME "roller_coaster")

add_dependencies(${PROJECT}_process_roller_coaster ${PROJECT}_process_person)

#==============  Rendezvous Thread Roller Coaster ===============================
add_executable(${PROJECT}_thread_roller_coaster src/thread/roller_coaster.cpp)
target_link_libraries(${PROJECT}_thread_roller_coaster Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(${PROJECT}_thread_roller_coaster rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_thread_roller_coaster PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/thread"
        OUTPUT_NAME "roller_coaster")
