# scope.cmake - Implements global target stack
#
# Needed to infer the currently build target when it's needed
# Some buildsystem APIs use this to inject scope when entering
# a library/executable (module/project/target/platform) target,
# for it's configuration to be able to link dependencies directly
# to that target

set(TARGET_SCOPE "")

macro(target_scope_enter name)
    list(APPEND TARGET_SCOPE ${name})
endmacro()

macro(target_scope_current res)
    list(GET TARGET_SCOPE -1 ${res})
endmacro()

macro(target_scope_exit)
    list(POP_BACK TARGET_SCOPE)
endmacro()
