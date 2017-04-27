cmake_minimum_required(VERSION 3.1.3)
project(q3_datapools)

set(CMAKE_CXX_STANDARD 11)

# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories("../include")

#==============  Datapool Child ===============================
add_executable(q3child1 src/q3child1.cpp)
target_link_libraries(q3child1 Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(q3child1 rt)
endif()

#==============  Datapool Parent  ======================================
add_executable(q3parent1 src/q3parent1.cpp)
target_link_libraries(q3parent1 Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(q3parent1 rt)
endif()
add_dependencies(q3parent1 q3child1)

#==============  Datapool Child ===============================
add_executable(q3child2 src/q3child2.cpp)
target_link_libraries(q3child2 Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(q3child2 rt)
endif()

#==============  Datapool Parent  ======================================
add_executable(q3parent2 src/q3parent2.cpp)
target_link_libraries(q3parent2 Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(q3parent2 rt)
endif()
add_dependencies(q3parent2 q3child2)

#==============  Datapool thread  ======================================
add_executable(q3thread src/q3thread.cpp)
target_link_libraries(q3thread Threads::Threads)