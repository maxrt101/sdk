
set(PLATFORM_SEARCH_PATHS ${SDK_DIR}/platform)

macro(platform_declare name)
    glob_files(platform_sources ${ARGN})
    add_library(platform_${name} ${platform_sources})
endmacro()

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

macro(platform_add_search_path path)
    list(APPEND PLATFORM_SEARCH_PATHS ${path})
endmacro()
