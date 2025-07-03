# =========================================================================
#
# @file utils.cmake
# @date 20-07-2024
# @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
#
# @brief Various utilities
#
# =========================================================================

include_guard(GLOBAL)

string(ASCII 27 ESC)
set(COLOR_RESET     "${ESC}[m")
set(COLOR_RED       "${ESC}[31m")
set(COLOR_GREEN     "${ESC}[32m")
set(COLOR_YELLOW    "${ESC}[33m")
set(COLOR_BLUE      "${ESC}[34m")
set(COLOR_MAGENTA   "${ESC}[35m")
set(COLOR_CYAN      "${ESC}[36m")
set(COLOR_WHITE     "${Esc}[37m")

#
# @brief Retrieves host OS type (Windows, Darwin or Linux)
#
# @param[out] out_os Where to put the result
#
function(get_host_os out_os)
    if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        set(${out_os} "Windows" PARENT_SCOPE)
    elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
        set(${out_os} "Darwin" PARENT_SCOPE)
    elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        set(${out_os} "Linux" PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Unsupported OS: ${CMAKE_HOST_SYSTEM_NAME}")
    endif ()
endfunction()

#
# @brief Retrieves full path to executable
#
# @param[in]  bin_name Name of the executable
# @param[out] out_path Where to put resulting path
#
macro(bin_which bin_name out_path)
    execute_process(COMMAND which ${bin_name}
            OUTPUT_VARIABLE ${out_path}
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro()

#
# @brief Crops filename out of a path
#
# @param[in]  file_path Path to file
# @param[out] out_path Where to put resulting path
#
macro(dir_name file_path out_path)
    execute_process(COMMAND dirname ${file_path}
            OUTPUT_VARIABLE ${out_path}
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro()

#
# @brief Parse name and value from feature declaration (NAME=VALUE)
#
# @param[in]  Feature declaration
# @param[out] Parsed feature name
# @param[out] Parsed feature value
#
macro(feature_parse_name_value feature name value)
    string(REPLACE "=" ";" feature_tokens ${feature})
    list(GET feature_tokens 0 ${name})
    list(GET feature_tokens 1 ${value})
endmacro()

#
# @brief Get feature value. Check CMake variables and ENV variables
#        if not defined - will use default value
#
# @param[in]  Feature declaration
# @param[out] Feature value
#
macro(feature_get_value feature value)
    if (DEFINED ${feature})
        set(${value} ${${feature}})
    elseif (DEFINED ENV{${feature}})
        set(${value} $ENV{${feature}})
    else()
        set(${value} UNDEFINED)
    endif ()
endmacro()

#
# @brief Parse all features in ${features}, adding definitions to compilation defines
#
# @param[in] features List of NAME=VALUE pairs
#
macro (feature_parse_all)
    foreach (feature ${ARGN})
        feature_parse_name_value(${feature} name default)
        feature_get_value(${name} val)

        if (${val} STREQUAL UNDEFINED)
            message(STATUS "${COLOR_BLUE}Feature ${name} using default value ${default}${COLOR_RESET}")
            project_add_define(ALL ${feature})
        else ()
            message(STATUS "${COLOR_YELLOW}Feature ${name} overridden, value ${val}${COLOR_RESET}")
            project_add_define(ALL ${name}=${val})
        endif ()
    endforeach ()
endmacro()
