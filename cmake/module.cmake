# module.cmake - SDK Modules support
#
# In the SDK shared functionality (may be called libraries),
# drivers, HAL, etc. are grouped under a uniform "module" concept.
# Modules are distinct separate pieces of software, that can be
# used by other modules, platforms, targets or projects
#
# Essentially all modules are compiled as static libraries, which
# are then linked to a higher scoped target (which required that
# specific module)
#

set(MODULE_SEARCH_PATHS ${SDK_DIR}/module)
set(MODULE_IMPORTED "")

# Macro for declaring a module
# Accepts a module name, and list of glob expressions/raw paths to source files
# Example:
#   .. in example/module.cmake
#   module_declare(example
#       ${MOD_DIR}/include/*.c
#       ${MOD_DIR}/src/*.c
#       ${MOD_DIR}/examples/example1.c
#   )
#
macro(module_declare name)
    glob_files(module_sources ${ARGN})
    add_library(${name} ${module_sources})
    set(MOD_NAME ${name})
endmacro()

# Implementation of requires() macro
#
macro(__requires name)
    set(found FALSE)

    target_scope_current(current)
    message(TRACE "'${current}' requires '${name}'")

    if (NOT ${name} IN_LIST MODULE_IMPORTED)
        foreach (path ${MODULE_SEARCH_PATHS})
            if (EXISTS "${path}/${name}/module.cmake")
                list(APPEND MODULE_IMPORTED ${name})

                set(MOD_DIR "${path}/${name}")

                target_scope_enter(${name})
                include("${path}/${name}/module.cmake")
                target_scope_exit()

                include_directories("${path}/${name}/include")

                if (ARGN)
                    set(target ${ARGN})
                else()
                    target_scope_current(target)
                endif()

                message(TRACE "Module '${name}' links to target '${target}'")
                target_link_libraries(${target} "$<LINK_LIBRARY:WHOLE_ARCHIVE,${name}>")

                add_definitions(-D__has_module_${name}=1)

                message(STATUS "${COLOR_GREEN}Found module '${name}' at ${path}/${name} ${COLOR_RESET}")

                set(found TRUE)

                break()
            endif()
        endforeach()
    else()
        message(DEBUG "${COLOR_YELLOW}Skip re-inclusion of module '${name}' ${COLOR_RESET}")
        set(found TRUE)
    endif()

    if (NOT found)
        message(FATAL_ERROR "Can't find module '${name}'")
    endif ()
endmacro()

# Main dependency management mechanism
# Accepts a list of required by current (or specified via optional
# TARGET parameter) target modules, evaluates and links them to current
# (or specified) target
# Example:
#   .. in example/module.cmake
#   module_declare(example ${MOD_DIR}/src/*.c)
#   requires(util tty hal)
#
macro(requires)
    cmake_parse_arguments(ARG "" "TARGET" "" ${ARGN})

    foreach (name ${ARG_UNPARSED_ARGUMENTS})
        __requires(${name} ${ARG_TARGET})
    endforeach ()
endmacro()

# Add a path to module search paths
#
macro(module_add_search_path path)
    list(APPEND MODULE_SEARCH_PATHS ${path})
endmacro()
