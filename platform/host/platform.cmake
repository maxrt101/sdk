
platform_declare(host
    ${PLATFORM_DIR}/include/*.h
    ${PLATFORM_DIR}/src/*.c
)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    features_declare(FORCE
        WITH_LOADER_ELF_CONTAINER=0
        WITH_LOADER_MACHO_CONTAINER=1
        LOADER_DEFAULT_CONTAINER=MACHO
    )
endif()
