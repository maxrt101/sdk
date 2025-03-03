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
# @param[in] bin_name Name of the executable
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
# @param[in] file_path Path to file
# @param[out] out_path Where to put resulting path
#
macro(dir_name file_path out_path)
    execute_process(COMMAND dirname ${file_path}
            OUTPUT_VARIABLE ${out_path}
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro()