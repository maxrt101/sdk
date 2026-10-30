module_declare(lds
    ${MOD_DIR}/include/*.h
    ${MOD_DIR}/src/*.c
)

requires(util)

features_declare(
    LDS_DYNAMIC=1
    LDS_MAX_SIZE=128
)
