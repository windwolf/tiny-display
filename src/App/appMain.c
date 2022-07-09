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
    .mode = MESSAGE_SCHEMA_MODE_FIXED_LENGTH,
    .prefix = {0xFF, 0xFE},
    .prefixSize = 2,
    .fixed.length = 14,
    .cmdLength = 0,
    .crc.length = 0,
    .alterDataSize = 0,
    .suffixSize = 0,
};
void appMain(void)
{
  ringbuffer_create(&uartRxRingBuffer, uart1_rx_buffer, 1, UART1_RX_BUFFER_SIZE);

  uart_device_create(&uart1Dev, &huart1, 4);
  uart_device_init(&uart1Dev);

  stream_create(&stream, &uart1Dev, &uartRxRingBuffer);
  message_parser_create(&mp, "demo", &schema, &uartRxRingBuffer);

  stream_server_start(&stream);

  MessageFrame frame;

  uint8_t fData[16];

  while (1)
  {
    stream_receive_ready_wait(&stream, 1000);
    while (message_parser_frame_get(&mp, NULL, &frame) == OP_RESULT_OK)
    {
      message_parser_frame_content_extract(&frame, fData);
    };
  }
}
