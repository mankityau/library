cmake_minimum_required(VERSION 3.1.3)
project(q6_rendezvous)

set(CMAKE_CXX_STANDARD 11)

# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories("../include")

#==============  Rendezvous Child ===============================
add_executable(q6child src/q6child.cpp)
target_link_libraries(q6child Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(q6child rt)
endif()

#==============  Rendezvous Parent  ==============================
add_executable(q6parent src/q6parent.cpp)
target_link_libraries(q6parent Threads::Threads)
# shared_memory requires linking with the rt library
if(UNIX)
    target_link_libraries(q6parent rt)
endif()
add_dependencies(q6parent q6child)

#==============  Rendezvous Thread  ==============================
add_executable(q6thread src/q6thread.cpp)
target_link_libraries(q6thread Threads::Threads)