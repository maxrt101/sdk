cmake_minimum_required(VERSION 3.27)

project(tests)

add_custom_target(tests_run)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/vfs)
