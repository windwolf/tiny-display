#include "ssd1306_demo.h"
#include "stm32f1xx_hal.h"
#include "common/device.h"
#include "bsp.h"
#include "config.h"
#include "ringbuffer.h"
#include "common/stream.h"
#include "message/message_parser.h"

static UartDevice uart1Dev;
extern UART_HandleTypeDef huart1;
static RingBuffer uartRxRingBuffer;
static Stream stream;
static uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
static MessageParser mp;
static MessageSchema schema = {
    .prefixSize = 0,
    .mode = MESSAGE_SCHEMA_MODE_FREE_LENGTH,
    .cmdLength = 0,
    .crc.length = 0,
    .alterDataSize = 0,
    .suffixSize = 2,
    .suffix = {'\r', '\n'},
};
void appMain(void)
{
  ringbuffer_create(&uartRxRingBuffer, uart1_rx_buffer, 1, UART1_RX_BUFFER_SIZE);

  uart_device_create(&uart1Dev, &huart1, 4);

  stream_create(&stream, &uart1Dev, &uartRxRingBuffer);
  message_parser_create(&mp, "demo", &schema, &uartRxRingBuffer);

  uart_device_init(&uart1Dev);
  uart_device_circular_rx_start(&uart1Dev, uart1_rx_buffer, UART1_RX_BUFFER_SIZE);

    {
    ssd1306_demo();
  }
}
