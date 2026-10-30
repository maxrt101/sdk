module_declare(loader
    ${MOD_DIR}/include/*.h
    ${MOD_DIR}/src/*.c
)

requires(util lds)

features_declare(
    WITH_LOADER_MACHO_CONTAINER=0
    WITH_LOADER_ELF_CONTAINER=1
    LOADER_DEFAULT_CONTAINER=ELF
)
