# =========================================================================
#
# @file platform.cmake
# @date 27-02-2024
# @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
#
# @brief Platform definition for STM32F1xx
#
# =========================================================================

include_guard(GLOBAL)

####################    VARIABLES    ####################
set(PLATFORM_NAME         "STM32F1xx")
set(PLATFORM_DIR          "${CMAKE_CURRENT_LIST_DIR}")
set(PLATFORM_SUPPORT_DIR  "${CMAKE_CURRENT_LIST_DIR}/../support/stm32")

####################    OPTIONS     ####################
project_add_define(
    "STM32F1xx"
    # Needed by ST HAL/LL
    "USE_HAL_DRIVER=1"
    "USE_FULL_LL_DRIVER=1"
)

####################   SOURCES    ####################
project_add_inc_dirs(
        "${PLATFORM_DIR}"
        "${PLATFORM_DIR}/Drivers/CMSIS/Include"
        "${PLATFORM_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include"
        "${PLATFORM_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc"
        "${PLATFORM_SUPPORT_DIR}"
)

project_add_src_recursive("${PLATFORM_DIR}/Drivers" "${PLATFORM_SUPPORT_DIR}")

message(STATUS "Using ${PLATFORM_NAME} platform (${PLATFORM_DIR})")