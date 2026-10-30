# project.cmake - Projects
#
# A project is a top-level target to which everything is linked: modules, targets
#

# Declare a project. Should be called after CMake project() was called in CMakeLists
# Same as module_declare, accepts a project name, and a list of glob paths/raw paths for sources
#
macro(project_declare name)
    glob_files(project_sources ${ARGN})
    add_executable(${name} ${project_sources})
    target_scope_enter(${name})
endmacro()

# Should be called at the end of CMakeLists
#
macro(project_finalize)
    features_finalize()
    apply_verbose()
    target_scope_exit()
endmacro()
