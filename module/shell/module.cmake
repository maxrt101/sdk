module_declare(shell
    ${MOD_DIR}/include/*.h
    ${MOD_DIR}/src/*.c
)

requires(util lds tty)

features_declare(
    SHELL_HISTORY_BUFFER_SIZE=16
    SHELL_MAX_LINE_SIZE=64
    SHELL_MAX_TOKENS=16
    SHELL_PROMPT="\$ "
    WITH_SHELL_HELP=1
    WITH_SHELL_HISTORY=1
)
