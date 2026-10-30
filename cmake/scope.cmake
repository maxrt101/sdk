
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
