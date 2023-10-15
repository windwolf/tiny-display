# TODO: 项目名称
set(PROJECT_NAME "TinyDisplay")

# TODO: 目标MCU
set(TARGET_MCU "STM32F103C8T6")
# set(TARGET_MCU "STM32G031C8T6")

option(USE_LL_LIB "Enable LL library" ON)
option(USE_HAL_LIB "Enable HAL library" ON)
option(USE_CMSIS_DSP_LIB "Enable CMSIS DSP library" ON)
option(USE_SYSTEM_VIEW "Enable Segger SystemView library" OFF)

option(ENABLE_DEMO "Enable demo" ON)

set(SYSCALL "RTT")
set(LOG_LEVEL "INFO")

set(EVENT_POOL_SIZE 2)

# set(OS "threadx")
# set(OS "nortos")
set(OS "freertos")
set(OS_PORT_FREERTOS_MEM_MANG "heap4")

