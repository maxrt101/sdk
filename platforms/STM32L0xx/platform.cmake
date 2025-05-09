# =========================================================================
#
# @file platform.cmake
# @date 19-09-2024
# @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
#
# @brief Platform definition for STM32L0xx
#
# =========================================================================

include_guard(GLOBAL)

####################    VARIABLES    ####################
set(PLATFORM_NAME         "STM32L0xx")
set(PLATFORM_DIR          "${CMAKE_CURRENT_LIST_DIR}")
set(PLATFORM_SUPPORT_DIR  "${CMAKE_CURRENT_LIST_DIR}/../support/stm32")

####################    OPTIONS     ####################
project_add_define(
    "STM32L0xx"
    # Needed by ST HAL/LL
    "USE_HAL_DRIVER=1"
    "USE_FULL_LL_DRIVER=1"
)

####################   SOURCES    ####################
project_add_inc_dirs(
        "${PLATFORM_DIR}"
        "${PLATFORM_DIR}/Drivers/CMSIS/Include"
        "${PLATFORM_DIR}/Drivers/CMSIS/Device/ST/STM32L0xx/Include"
        "${PLATFORM_DIR}/Drivers/STM32L0xx_HAL_Driver/Inc"
        "${PLATFORM_SUPPORT_DIR}"
)

project_add_src_recursive("${PLATFORM_DIR}/Drivers" "${PLATFORM_SUPPORT_DIR}")

message(STATUS "Using ${PLATFORM_NAME} platform (${PLATFORM_DIR})")