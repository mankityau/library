cmake_minimum_required(VERSION 3.1.3)
project(q6_condition)

set(CMAKE_CXX_STANDARD 11)

# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories("../include")

#==============  Condition Car ===============================
add_executable(q6car src/q6car.cpp)
target_link_libraries(q6car Threads::Threads)
if(UNIX)
    target_link_libraries(q6car rt)
endif()


#==============  Condition Pedestrian =======================
add_executable(q6pedestrian src/q6pedestrian.cpp)
target_link_libraries(q6pedestrian Threads::Threads)
if(UNIX)
    target_link_libraries(q6pedestrian rt)
endif()


#==============  Condition Parent  ==============================
add_executable(q6intersection src/q6intersection.cpp)
target_link_libraries(q6intersection Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(q6intersection rt)
endif()
add_dependencies(q6intersection q6car q6pedestrian)

#==============  Condition Thread  ==============================
add_executable(q6conditionthread src/q6conditionthread.cpp)
target_link_libraries(q6conditionthread Threads::Threads)