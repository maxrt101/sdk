cmake_minimum_required(VERSION 3.27)

project(vfs_tests C)

set(SDK_DIR "${CMAKE_CURRENT_LIST_DIR}/../../")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-I ${SDK_DIR} -I ${SDK_DIR}/lib")

add_definitions(
    -DVFS_ALLOC_INC="trace_alloc/trace_alloc.h"
    -DVFS_ALLOC=trace_alloc
    -DVFS_FREE=trace_free
    -DTRACE_ALLOC_PORT_ALLOC_FN=malloc
    -DTRACE_ALLOC_PORT_FREE_FN=free
    -DTRACE_ALLOC_PORT_ALLOC_INC="stdlib.h"
    -DTRACE_ALLOC_PORT_LOG=printf
    -DTRACE_ALLOC_PORT_LOG_INC="stdio.h"
    -DTEST_LOG_PORT=printf
    -DTEST_LOG_PORT_INC="stdio.h"
    -D__STATIC_INLINE=
)

add_executable(vfs_tests
    ${CMAKE_CURRENT_LIST_DIR}/vfs_tests.c
    ${SDK_DIR}/lib/vfs/vfs.c
    ${SDK_DIR}/lib/table/table.c
    ${SDK_DIR}/lib/test/test.c
    ${SDK_DIR}/lib/trace_alloc/trace_alloc.c
)

add_custom_target(vfs_tests_run
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vfs_tests
)

add_dependencies(tests_run vfs_tests_run)
