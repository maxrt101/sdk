# =========================================================================
#
# @file compiler.cmake
# @date 20-07-2024
# @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
#
# @brief Compiler setup
#
# =========================================================================

include_guard(GLOBAL)

#
# @brief Retrieves compiler root folder
#
# @param[in] bin compiler binary name
# @param[out] folder Where to put resulting path
#
macro(compiler_get_folder bin folder)
    bin_which(${bin} bin_path)
    if ("${bin_path}" MATCHES "not found" OR "${bin_path}" STREQUAL "")
        message(FATAL_ERROR "Compiler executable not found (${bin})")
    endif ()
    dir_name(${bin_path} compiler_path)
    set(${folder} ${compiler_path})
endmacro()

#
# @brief Setup compiler in cmake
#
# @param[in] family Compiler family (GCC, CLANG, etc)
# @param[out] folder Compiler prefix (e.g. for arm gcc its arm-none-eabi)
#
macro(compiler_setup family prefix)
    if (NOT ${family} STREQUAL GCC)
        message(FATAL_ERROR "Unsupported compiler family ${family}")
    endif ()

    set(bin ${prefix}-gcc)

    compiler_get_folder(${bin} compiler_folder)

    message(STATUS "Setting up compiler (${family} at ${compiler_folder})")

    set(CMAKE_C_COMPILER_ID ${CMAKE_COMPILER_ID} CACHE STRING "" FORCE)
    set(CMAKE_C_COMPILER_VERSION ${version} CACHE STRING "" FORCE)
    set(CMAKE_C_COMPILER "${compiler_folder}/${prefix}-gcc" CACHE STRING "" FORCE)

    set(CMAKE_C_STANDARD 11 CACHE STRING "" FORCE)
    set(CMAKE_C_STANDARD_COMPUTED_DEFAULT 11 CACHE STRING "" FORCE)
    set(CMAKE_C_STANDARD_REQUIRED ON CACHE STRING "" FORCE)
    set(CMAKE_C_EXTENSIONS ON CACHE STRING "" FORCE)

    set(CMAKE_CXX_COMPILER_ID ${CMAKE_COMPILER_ID} CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER_VERSION ${version} CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER "${compiler_folder}/${prefix}-g++" CACHE STRING "" FORCE)

    set(CMAKE_CXX_STANDARD 14 CACHE STRING "" FORCE)
    set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT 14 CACHE STRING "" FORCE)
    set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING "" FORCE)
    set(CMAKE_CXX_EXTENSIONS ON CACHE STRING "" FORCE)

    set(CMAKE_ASM_COMPILER_ID ${CMAKE_COMPILER_ID} CACHE STRING "" FORCE)
    set(CMAKE_ASM_COMPILER_VERSION ${version} CACHE STRING "" FORCE)
    set(CMAKE_ASM_COMPILER "${compiler_folder}/${prefix}-gcc" CACHE STRING "" FORCE)

    macro(__compiler_setup_executable name variable)
        set(bin "${compiler_folder}/${prefix}-${name}")
        if (NOT (bin MATCHES NOTFOUND) AND NOT (bin STREQUAL ""))
            set(${variable} "${bin}" CACHE STRING "" FORCE)
        else ()
            message(WARNING "${name} binary - not found")
        endif ()
    endmacro()

    __compiler_setup_executable(ld CMAKE_LD)
    __compiler_setup_executable(objcopy CMAKE_OBJCOPY)
    __compiler_setup_executable(size CMAKE_SIZE)
    __compiler_setup_executable(gdb CMAKE_GDB)
    __compiler_setup_executable(ar CMAKE_AR)
    __compiler_setup_executable(strip CMAKE_STRIP)
    __compiler_setup_executable(readelf CMAKE_READELF)
    __compiler_setup_executable(objdump CMAKE_OBJDUMP)
    __compiler_setup_executable(gcov CMAKE_GCOV)

    set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf CACHE STRING "" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX_C .elf CACHE STRING "" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf CACHE STRING "" FORCE)

    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "" CACHE STRING "" FORCE)

    set(CMAKE_SYSTEM_NAME Generic CACHE STRING "" FORCE)
    set(CMAKE_SYSTEM_PROCESSOR arm CACHE STRING "" FORCE)
endmacro()

macro(compiler_generate_hex)
    message(STATUS "Adding HEX generation")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> \"${PROJECT_NAME}.hex\")
endmacro()

macro(compiler_generate_bin)
    message(STATUS "Adding BIN generation")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> \"${PROJECT_NAME}.bin\")
endmacro()

macro(compiler_generate_map)
    message(STATUS "Adding MAP generation")
    project_add_link_options(ALL
            -Wl,--cref,-Map=${PROJECT_NAME}.map
    )
endmacro()

macro(compiler_show_size)
    message(STATUS "Adding binary size displaying")
    project_add_link_options(ALL
            -Wl,--print-memory-usage
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>)
endmacro()
