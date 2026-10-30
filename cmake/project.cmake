
macro(project_declare name)
    glob_files(project_sources ${ARGN})
    add_executable(${name} ${project_sources})
    target_scope_enter(${name})
endmacro()

macro(project_finalize)
    features_finalize()
    apply_verbose()
    target_scope_exit()
endmacro()
