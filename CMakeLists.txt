#==========================================================
#  CMAKE Setup
#==========================================================

cmake_minimum_required(VERSION 3.1.3)
project(labs)

# customize binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # separate binaries from build files

# set default install prefix to source directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "default install path" FORCE )
endif()

set(MY_OUTPUT_DIR "")      # blank output directory

# include Doxygen documentation support
include(Doxygen.cmake)

# custom macros
include(Macros.cmake)
warnall()  # enable all warnings

add_subdirectory("examples")
add_subdirectory("tools")

#==========================================================
#  Actual Labs
#==========================================================


#==========================================================
#  Testing and Install
#==========================================================

# Tests
# Enable top-level testing
# include (CTest)
enable_testing()
add_subdirectory("test")

# install header library if not current install directory
if (NOT ${CMAKE_INSTALL_PREFIX} EQUAL ${CMAKE_SOURCE_DIR} )
    install(DIRECTORY include/cpen333 DESTINATION include/)
endif()