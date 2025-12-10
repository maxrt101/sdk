
macro(freertos_configure cfg)
  include(FetchContent)

  FetchContent_Declare(
      FreeRTOS_Kernel
      GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
      GIT_TAG        V11.2.0
  )

  add_library(freertos_config INTERFACE)
  target_include_directories(freertos_config SYSTEM INTERFACE ${cfg})

  message(STATUS "Setting up FreeRTOS")
  message(STATUS "FreeRTOS port: ${FREERTOS_PORT}")
  message(STATUS "FreeRTOS heap: ${FREERTOS_HEAP}")

  FetchContent_MakeAvailable(FreeRTOS_Kernel)

  list(APPEND freertos_targets freertos_kernel freertos_config freertos_kernel_port)

  foreach (target ${freertos_targets})
    target_compile_definitions(${target} INTERFACE ${PROJECT_DEFINES})
    target_compile_options(${target} INTERFACE ${PROJECT_COMPILE_FLAGS})
    target_link_options(${target} INTERFACE ${PROJECT_LINK_FLAGS})
    target_include_directories(${target} INTERFACE ${PROJECT_INCLUDE_DIRS})
  endforeach ()

  target_link_libraries(${PROJECT_NAME} freertos_kernel)
endmacro()
