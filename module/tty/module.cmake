module_declare(tty
    ${MOD_DIR}/include/*.h
    ${MOD_DIR}/src/*.c
)

requires(util lds device)

# TODO: WITH_TTY_ANSI
# TODO: line ending?
features_declare(
    WITH_TTY_EXTENDED_ANSI=1
    WITH_TTY_ANSI_COLORS=1
    WITH_TTY_COLORTEST=1
    TTY_MAX_LINE_SIZE=128
)
