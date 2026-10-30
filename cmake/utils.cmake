
macro(glob_files res)
    set(${res} "")

    foreach (path ${ARGN})
        unset(found_src)
        string(FIND "${path}" "*" asterisk_index)

        if (NOT asterisk_index EQUAL -1)
            file(GLOB_RECURSE found_src ${path})
        else()
            set(found_src ${path})
        endif()

        list(APPEND ${res} ${found_src})
    endforeach ()
endmacro()

macro(apply_verbose)
    if ("${FEATURE_VERBOSE}" STREQUAL "1")
        message(STATUS "${COLOR_MAGENTA}Verbose ON${COLOR_RESET}")
        set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Verbose makefile" FORCE)
        set(CMAKE_MESSAGE_LOG_LEVEL "TRACE" CACHE STRING "CMake Log level")
    endif ()
endmacro()
