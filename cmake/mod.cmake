# mod.cmake - Create loadable shared PIC modules
#
# mod_declare will create a shared library target
# which can be embedded into the main application
# and be loaded (and relocs patched) in runtime
#
# type - is executable container type, possible values:
#   MACHO - For Darwin (MacOS/IOS/etc.)
#   ELF   - for Linux and embedded targets
#
# For more info look at loader module
#

macro(mod_declare type name)
    glob_files(sources ${ARGN})

    add_library(${name} MODULE ${sources})

    set_target_properties(${name} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        PREFIX ""
    )

    target_compile_options(${name} PRIVATE
        -Os
        -ffunction-sections
        -fdata-sections
        # For all symbols to be hidden by default
        # -fvisibility=hidden
    )

    if ("${type}" STREQUAL "MACHO")
        target_link_options(${name} PRIVATE
            -Wl,-dead_strip
            -Wl,-undefined,dynamic_lookup
        )

        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND strip -x $<TARGET_FILE:${name}>
            COMMENT "Stripping Mach-O module: ${name}"
        )
    elseif ("${type}" STREQUAL "ELF")
        target_link_options(${name} PRIVATE
            -Wl,--gc-sections
            -Wl,--unresolved-symbols=ignore-all
        )

        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND strip --strip-unneeded $<TARGET_FILE:${name}>
            COMMENT "Stripping ELF module: ${name}"
        )
    else()
        message(FATAL_ERROR "Invalid loadable mod type: '${type}'")
    endif()

    message(STATUS "${COLOR_CYAN}Including loadable ${type} mod '${name}'${COLOR_RESET}")
endmacro()
