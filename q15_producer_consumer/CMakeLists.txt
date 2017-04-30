cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Baker ===============================
add_executable(${PROJECT}_baker src/baker.cpp)

# add required libraries
target_link_libraries(${PROJECT}_baker Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_baker rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_baker PROPERTIES
        OUTPUT_NAME "Baker")

#==============  Cookie Monster ===============================
add_executable(${PROJECT}_cookie_monster src/cookie_monster.cpp)

# add required libraries
target_link_libraries(${PROJECT}_cookie_monster Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_cookie_monster rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_cookie_monster PROPERTIES
        OUTPUT_NAME "cookie_monster")

#==============  Main Bakery ===============================
add_executable(${PROJECT}_bakery src/bakery.cpp)

# add required libraries
target_link_libraries(${PROJECT}_bakery Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_bakery rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_bakery PROPERTIES
        OUTPUT_NAME "bakery")

add_dependencies(${PROJECT}_bakery ${PROJECT}_baker ${PROJECT}_cookie_monster)