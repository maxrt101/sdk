# feature.cmake - Overridable compile-time features
#
# readme TBD
#

set(DEFINED_FEATURES "")

macro(feature_parse_name_value feature name value)
    string(REPLACE "=" ";" feature_tokens ${feature})
    list(GET feature_tokens 0 ${name})
    list(GET feature_tokens 1 ${value})
endmacro()

macro(feature_get_value value feature default)
    if (DEFINED ${feature})
        set(${value} ${${feature}})
    elseif (DEFINED ENV{${feature}})
        set(${value} $ENV{${feature}})
    else()
        set(${value} ${default})
    endif ()
endmacro()

macro(features_declare)
    cmake_parse_arguments(ARG "FORCE" "" "" ${ARGN})

    foreach (feature ${ARG_UNPARSED_ARGUMENTS})
        feature_parse_name_value(${feature} name default)
        feature_get_value(val ${name} ${default})

        if (NOT ${name} IN_LIST DEFINED_FEATURES OR ARG_FORCE)
            if (${name} IN_LIST DEFINED_FEATURES AND ARG_FORCE)
                message(STATUS "${COLOR_YELLOW}Feature ${name} force redefined to ${val} ${COLOR_RESET}")
            else()
               message(DEBUG "${COLOR_BLUE}Feature ${name} value ${val}${COLOR_RESET}")
               list(APPEND DEFINED_FEATURES ${name})
            endif()

            set(FEATURE_${name} ${val})
        else()
            message(DEBUG "${COLOR_YELLOW}Feature ${name} already defined${COLOR_RESET}")
        endif()
    endforeach ()
endmacro()

macro(features_finalize)
    foreach (feature ${DEFINED_FEATURES})
        add_compile_definitions(${feature}=${FEATURE_${feature}})
    endforeach ()
endmacro()
