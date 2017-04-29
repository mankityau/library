cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)            # create new project with name PROJECT

set(CMAKE_CXX_STANDARD 11)  # use C++11 standard

# customize output directory to be within a "bin/<project>" folder, relative to the BASE CMakeLists.txt file
#   (i.e. if this project is added to another top-level one, the bin folder will be relative to the top level)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT})

# directories to search for header files
include_directories(src)

#==============  Objects ===============================
# create executable with unique target label, add all sources
add_executable(${PROJECT}_objects src/objects/main.cpp src/objects/Animal.cpp src/objects/Cat.cpp src/objects/Dog.cpp)
target_include_directories(${PROJECT}_objects PRIVATE src/objects)  # local headers

# customize output name (otherwise would be given target name by default)
set_target_properties(${PROJECT}_objects  PROPERTIES
        OUTPUT_NAME "objects")

#==============  Namespaces ===============================
add_executable(${PROJECT}_namespaces src/namespaces/main.cpp src/namespaces/foo.cpp src/namespaces/bar.cpp)
target_include_directories(${PROJECT}_namespaces PRIVATE src/namespaces)  # local headers

# customize output name (otherwise would be given target name by default)
set_target_properties(${PROJECT}_namespaces  PROPERTIES
        OUTPUT_NAME "namespaces")

#==============  RAII ===============================
add_executable(${PROJECT}_raii src/RAII/main.cpp)

# customize output name (otherwise would be given target name by default)
set_target_properties(${PROJECT}_raii  PROPERTIES
        OUTPUT_NAME "raii")

#==============  Templates ===============================
add_executable(${PROJECT}_templates src/templates/main.cpp)

# customize output name (otherwise would be given target name by default)
set_target_properties(${PROJECT}_templates  PROPERTIES
        OUTPUT_NAME "templates")

#==============  Containers ===============================
add_executable(${PROJECT}_containers src/containers/main.cpp)

# customize output name (otherwise would be given target name by default)
set_target_properties(${PROJECT}_containers  PROPERTIES
        OUTPUT_NAME "containers")

#==============  Advanced ===============================
add_executable(${PROJECT}_advanced src/advanced/main.cpp)

# customize output name (otherwise would be given target name by default)
set_target_properties(${PROJECT}_advanced  PROPERTIES
        OUTPUT_NAME "advanced")