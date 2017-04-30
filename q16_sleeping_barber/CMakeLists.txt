cmake_minimum_required(VERSION 3.1.3)

# Name project based on current directory
get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${PROJECT})

set(CMAKE_CXX_STANDARD 11)
find_package( Threads REQUIRED )  # Multithreading package required

# directories to search for header files
include_directories(../include)

#==============  Barber ===============================
add_executable(${PROJECT}_barber src/barber.cpp)

# add required libraries
target_link_libraries(${PROJECT}_barber Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_barber rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_barber PROPERTIES
        OUTPUT_NAME "barber")

#==============  Customer ===============================
add_executable(${PROJECT}_customer src/customer.cpp)

# add required libraries
target_link_libraries(${PROJECT}_customer Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_customer rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_customer PROPERTIES
        OUTPUT_NAME "customer")


#==============  Barber ===============================
add_executable(${PROJECT}_barber_shop src/barber_shop.cpp)

# add required libraries
target_link_libraries(${PROJECT}_barber_shop Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_barber_shop rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_barber_shop PROPERTIES
        OUTPUT_NAME "barber_shop")

#==============  Main ===============================
add_executable(${PROJECT}_sleeping_barbers src/sleeping_barbers.cpp)

# add required libraries
target_link_libraries(${PROJECT}_sleeping_barbers Threads::Threads)
if(UNIX)
    target_link_libraries(${PROJECT}_sleeping_barbers rt)
endif()

# set output directory and executable name
set_target_properties( ${PROJECT}_sleeping_barbers PROPERTIES
        OUTPUT_NAME "sleeping_barbers")

add_dependencies(${PROJECT}_sleeping_barbers ${PROJECT}_barber ${PROJECT}_customer ${PROJECT}_barber_shop)