# TODO: 项目名称
set(PROJECT_NAME "startup")

# TODO: 目标MCU
set(TARGET_MCU "STM32F103C8T6")

option(USE_LL_LIB "Enable LL library" ON)
option(USE_HAL_LIB "Enable HAL library" ON)
option(USE_CMSIS_DSP_LIB "Enable CMSIS DSP library" ON)
option(USE_SYSTEM_VIEW "Enable Segger SystemView library" OFF)

option(USE_RTT_PRINT "Enable RTT log" ON)
option(USE_UART_PRINT "Enable UART log" OFF)

option(ENABLE_DEMO "Enable demo" ON)

# set(OS_PORT "threadx")
# set(OS_PORT "nortos")
set(OS_PORT "freertos")
set(OS_PORT_FREERTOS_MEM_MANG "heap4")

# set(BSP_PORT "stm32h750")
set(BSP_PORT "stm32f1xx")
