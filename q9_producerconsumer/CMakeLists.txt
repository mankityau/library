cmake_minimum_required(VERSION 3.1.3)
project(q9_producerconsumer)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})


# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Baker ===============================
add_executable(q9_producerconsumer_baker src/Baker.cpp)

# add required libraries
target_link_libraries(q9_producerconsumer_baker Threads::Threads)
if(UNIX)
    target_link_libraries(q9_producerconsumer_baker rt)
endif()

# set output directory and executable name
set_target_properties( q9_producerconsumer_baker PROPERTIES
        OUTPUT_NAME "Baker")

#==============  Cookie Monster ===============================
add_executable(q9_producerconsumer_monster src/CookieMonster.cpp)

# add required libraries
target_link_libraries(q9_producerconsumer_monster Threads::Threads)
if(UNIX)
    target_link_libraries(q9_producerconsumer_monster rt)
endif()

# set output directory and executable name
set_target_properties( q9_producerconsumer_monster PROPERTIES
        OUTPUT_NAME "CookieMonster")

#==============  Main ===============================
add_executable(q9_producerconsumer_main src/main.cpp)

# add required libraries
target_link_libraries(q9_producerconsumer_main Threads::Threads)
if(UNIX)
    target_link_libraries(q9_producerconsumer_main rt)
endif()

# set output directory and executable name
set_target_properties( q9_producerconsumer_main PROPERTIES
        OUTPUT_NAME "main")

add_dependencies(q9_producerconsumer_main q9_producerconsumer_baker q9_producerconsumer_monster)