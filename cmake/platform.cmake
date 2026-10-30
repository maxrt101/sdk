# platform.cmake - Target platforms
#
# A platform is a hardware(or not) platform, for which a target is built
# A mechanism for platforms are heavily inspired by module.cmake
# Platforms use the same mechanism as targets
#

set(PLATFORM_SEARCH_PATHS ${SDK_DIR}/platform)

# Declare a platform - syntax is the same as module_declare/target_declare
# For more info, see module_declare()
#
macro(platform_declare name)
    glob_files(platform_sources ${ARGN})
    add_library(platform_${name} ${platform_sources})
endmacro()

# Include a platform into current cmake target
# For more info, see requires()
#
macro(platform_use name)
    cmake_parse_arguments(ARG "" "TARGET" "" ${ARGN})

    set(found FALSE)

    foreach (path ${PLATFORM_SEARCH_PATHS})
        if (EXISTS "${path}/${name}/platform.cmake")
            set(PLATFORM_DIR "${path}/${name}")

            target_scope_enter(platform_${name})
            include("${path}/${name}/platform.cmake")
            target_scope_exit()

            include_directories("${path}/${name}/include")

            if (ARG_TARGET)
                set(target ${ARG_TARGET})
            else()
                target_scope_current(target)
            endif()

            message(TRACE "Platform '${name}' links to target '${target}'")

            target_link_libraries(${target} "$<LINK_LIBRARY:WHOLE_ARCHIVE,platform_${name}>")

            add_definitions(-D__is_platform_${name}=1)

            message(STATUS "${COLOR_BLUE}Use platform '${name}' at ${path}/${name}${COLOR_RESET}")

            set(found TRUE)

            break()
        endif ()
    endforeach ()

    if (NOT found)
        message(FATAL_ERROR "Couldn't find platform '${name}'")
    endif()
endmacro()

# Add a path to target search paths
#
macro(platform_add_search_path path)
    list(APPEND PLATFORM_SEARCH_PATHS ${path})
endmacro()
