cmake_minimum_required(VERSION 3.1.3)
project(q11_sleepingbarber)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})


# Multithreading
find_package( Threads REQUIRED )

# library directories
include_directories(../include src)

#==============  Barber ===============================
add_executable(q11_sleepingbarber_barber src/Barber.cpp)

# add required libraries
target_link_libraries(q11_sleepingbarber_barber Threads::Threads)
if(UNIX)
    target_link_libraries(q11_sleepingbarber_barber rt)
endif()

# set output directory and executable name
set_target_properties( q11_sleepingbarber_barber PROPERTIES
        OUTPUT_NAME "Barber")

#==============  Customer ===============================
add_executable(q11_sleepingbarber_customer src/Customer.cpp)

# add required libraries
target_link_libraries(q11_sleepingbarber_customer Threads::Threads)
if(UNIX)
    target_link_libraries(q11_sleepingbarber_customer rt)
endif()

# set output directory and executable name
set_target_properties( q11_sleepingbarber_customer PROPERTIES
        OUTPUT_NAME "Customer")


#==============  Barber ===============================
add_executable(q11_sleepingbarber_barbershop src/BarberShop.cpp)

# add required libraries
target_link_libraries(q11_sleepingbarber_barbershop Threads::Threads)
if(UNIX)
    target_link_libraries(q11_sleepingbarber_barbershop rt)
endif()

# set output directory and executable name
set_target_properties( q11_sleepingbarber_barbershop PROPERTIES
        OUTPUT_NAME "BarberShop")

#==============  Main ===============================
add_executable(q11_sleepingbarber_main src/main.cpp)

# add required libraries
target_link_libraries(q11_sleepingbarber_main Threads::Threads)
if(UNIX)
    target_link_libraries(q11_sleepingbarber_main rt)
endif()

# set output directory and executable name
set_target_properties( q11_sleepingbarber_main PROPERTIES
        OUTPUT_NAME "main")

add_dependencies(q11_sleepingbarber_main q11_sleepingbarber_barber q11_sleepingbarber_customer q11_sleepingbarber_barbershop)