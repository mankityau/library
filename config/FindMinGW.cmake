# Find MINGW libraries and include paths
#
# This module defines
#  MINGW_INCLUDE_DIRS
#  MINGW_SEARCH_DIRS  - potential root directories
#  MINGW_ROOT         - main root directory
#  MINGW_FOUND

# find MinGW parent path using the executable
find_program(MINGW_EXE_PATH mingw32-make)
if ( MINGW_EXE_PATH )
    get_filename_component(MINGW_BIN_PATH ${MINGW_EXE_PATH} DIRECTORY)
    get_filename_component(MINGW_ROOT ${MINGW_BIN_PATH} DIRECTORY)

    set(MINGW_SEARCH_DIRS "${MINGW_ROOT}" "${MINGW_ROOT}/x86_64-w64-mingw32"
            "${MINGW_ROOT}/i686-w64-mingw32" "${MINGW_ROOT}/mingw32")

    find_path(MINGW_INCLUDE_DIR
            NAMES _mingw.h
            PATHS ${MINGW_SEARCH_DIRS}
            PATH_SUFFIXES "include"
            )

    if(MINGW_INCLUDE_DIR)
        set(MINGW_FOUND 1)

        set(MINGW_INCLUDE_DIRS ${MINGW_INCLUDE_DIR})
        foreach(f ${MINGW_SEARCH_DIRS})
            list(APPEND MINGW_INCLUDE_DIRS "${f}/include")
        endforeach()
    else()
        SET(MINGW_FOUND 0)
        SET(MINGW_INCLUDE_DIRS)
    endif()

endif()

# Report the results.
if(NOT MINGW_FOUND)
    if(NOT MINGW_FIND_QUIETLY)
        message(STATUS  "MinGW was not found. Make sure it is on the PATH.")
    else()
        if(MINGW_FIND_REQUIRED)
            message(FATAL_ERROR "MinGW was NOT found and is required by this project.  Make sure it is on the PATH.")
        endif()
    endif()
endif()

# Add MinGW directories to CMAKE_PREFIX_PATH so that libraries and headers can be found
macro(add_mingw_search_dirs)
    if (MINGW_ROOT)
        # Bug? http://public.kitware.com/pipermail/cmake/2015-December/062166.html
        set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)
        list(APPEND CMAKE_PREFIX_PATH "${MINGW_SEARCH_DIRS}")
    endif()
endmacro()

