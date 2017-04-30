cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT})            # create new project with name PROJECT

set(CMAKE_CXX_STANDARD 11)  # use C++11 standard

# customize output directory to be within a "bin/<project>" folder, relative to the BASE CMakeLists.txt file
#   (i.e. if this project is added to another top-level one, the bin folder will be relative to the top level)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

# Multithreading package required
find_package( Threads REQUIRED )

# directories to search for header files
include_directories(../include)

#==============  Basics ===============================
add_executable(${PROJECT}_basics src/basics/main.cpp)
target_link_libraries(${PROJECT}_basics Threads::Threads)  # link library to target

# customize output name (otherwise would be given target name by default)
set_target_properties(${PROJECT}_basics PROPERTIES
        OUTPUT_NAME "basics")

#==============  Object Oriented ===============================
add_executable(${PROJECT}_objectoriented src/objectoriented/main.cpp)
target_link_libraries(${PROJECT}_objectoriented Threads::Threads)  # link library to target

# customize output name (otherwise would be given target name by default)
set_target_properties(${PROJECT}_objectoriented  PROPERTIES
        OUTPUT_NAME "objectoriented")
