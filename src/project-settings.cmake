# TODO: 项目名称
set(PROJECT_NAME "TinyDisplay")

# TODO: 目标MCU
set(TARGET_MCU "STM32F103C8T6")
# set(TARGET_MCU "STM32G031C8T6")

option(USE_LL_LIB "Enable LL library" ON)
option(USE_HAL_LIB "Enable HAL library" ON)
option(USE_CMSIS_DSP_LIB "Enable CMSIS DSP library" ON)
option(USE_SYSTEM_VIEW "Enable Segger SystemView library" OFF)

option(USE_RTT_PRINT "Enable RTT log" ON)
option(USE_UART_PRINT "Enable UART log" OFF)

option(ENABLE_DEMO "Enable demo" ON)

# set(OS "threadx")
# set(OS "nortos")
set(OS "freertos")
set(OS_PORT_FREERTOS_MEM_MANG "heap4")

