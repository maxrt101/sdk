
set(TARGET_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/target)

macro(target_declare name)
    glob_files(target_sources ${ARGN})
    add_library(target_${name} ${target_sources})
endmacro()

macro(target_use name)
    cmake_parse_arguments(ARG "" "TARGET" "" ${ARGN})

    set(found FALSE)

    foreach (path ${TARGET_SEARCH_PATHS})
        if (EXISTS "${path}/${name}/target.cmake")
            set(PLATFORM_DIR "${path}/${name}")

            target_scope_enter(target_${name})
            include("${path}/${name}/target.cmake")
            target_scope_exit()

            include_directories("${path}/${name}/include")

            if (ARG_TARGET)
                set(target ${ARG_TARGET})
            else()
                target_scope_current(target)
            endif()

            message(TRACE "Target '${name}' links to target '${target}'")

            target_link_libraries(${target} "$<LINK_LIBRARY:WHOLE_ARCHIVE,target_${name}>")

            add_definitions(-D__is_target_${name}=1)

            message(STATUS "${COLOR_BLUE}Use target '${name}' at ${path}/${name}${COLOR_RESET}")

            set(found TRUE)

            break()
        endif ()
    endforeach ()

    if (NOT found)
        message(FATAL_ERROR "Couldn't find target '${name}'")
    endif()
endmacro()

macro(target_add_search_path path)
    list(APPEND TARGET_SEARCH_PATHS ${path})
endmacro()
