#include "stm32f1xx_hal.h"
#include "common/device.h"
#include "bsp.h"
#include "config.h"
#include "ringbuffer.h"
#include "common/stream.h"
#include "message/message_parser.h"
#include "appMain.h"
#include "string.h"
#include "device/ssd1306.h"
#include "fonts/fonts.h"

#define LOG_MODULE "appMain"
#include "log.h"

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

PinDevice scl;
PinDevice sda;
SoftI2CDevice softi2c;
I2CDevice i2c;
SSD1306 ZJ0_91in = {
    .width = 128,
    .height = 32,
    .memoryMode = SSD1306_MEMORY_ADDRESSING_MODE_VERTICAL,
    .enableChargePump = true,
    .comInverted = true,
    .segmentInverted = true,
    .comLeftRightRemap = false,
    .comAlternative = false,
    .displayInverted = false,
    .displayStartLine = 32,
    .displayOffset = 0,
    .multiplexRatio = 63,
    .phase1period = 0x02,
    .phase2period = 0x02,
    .vcomhDeselectLevel = 0x40,
    .fosc = 0x0F,
    .clkDivide = 0x00,
};

CanvasInfo canvasInfo = {
    .width = 128,
    .height = 32,
    .direction = FONT_MEMORY_LAYOUT_DIRECTION_VERTICAL,
    .pixelSize = PIXEL_SIZE_1BIT,
};
FontDrawInfo fontDrawInfo = {
    .backColor = 0x0,
    .foreColor = 0x1,
    .lineSpacing = 1,
    .spacing = 0,
};

void setup(void)
{

  pin_device_create(&scl, GPIOB, GPIO_PIN_6, PIN_DEVICE_STATUS_INVERSE_NORMAL);
  pin_device_create(&sda, GPIOB, GPIO_PIN_7, PIN_DEVICE_STATUS_INVERSE_NORMAL);
  softi2c.scl = &scl;
  softi2c.sda = &sda;
  i2c_device_create(&i2c, &softi2c);
  ssd1306_create(&ZJ0_91in, &i2c);
  ssd1306_init(&ZJ0_91in);

  ringbuffer_create(&uartRxRingBuffer, uart1_rx_buffer, 1, UART1_RX_BUFFER_SIZE);

  uart_device_create(&uart1Dev, &huart1, 4);
  uart_device_init(&uart1Dev);

  stream_create(&stream, &uart1Dev, &uartRxRingBuffer);
  message_parser_create(&mp, "demo", &schema, &uartRxRingBuffer);

  stream_server_start(&stream);
}

void loop(void)
{
  uint16_t rssi;
  uint16_t rssi1;
  uint16_t rssi2;
  uint16_t rssi3;
  uint16_t rssi4;
  bool get = false;
  MessageFrame frame;
  uint8_t fData[16];

  stream_receive_ready_wait(&stream, osWaitForever);
  while (message_parser_frame_get(&mp, NULL, &frame) == OP_RESULT_OK)
  {
    message_parser_frame_extract(&frame, fData);
    rssi = (fData[2] << 8) | fData[3];
    rssi1 = (fData[4] << 8) | fData[5];
    rssi2 = (fData[6] << 8) | fData[7];
    rssi3 = (fData[8] << 8) | fData[9];
    rssi4 = (fData[10] << 8) | fData[11];
    get = true;
  };

  if (get)
  {
    char str[22] = {0};
    ssd1306_clear(&ZJ0_91in);

    sprintf(str, "rssi=%d", rssi);
    FONTS_FillData(ZJ0_91in.data_buffer, &canvasInfo, 0, 0, str, &Font6x8_v, &fontDrawInfo);

    sprintf(str, "r1=%d r2=%d", rssi1, rssi2);
    FONTS_FillData(ZJ0_91in.data_buffer, &canvasInfo, 0, 8, str, &Font6x8_v, &fontDrawInfo);

    sprintf(str, "r3=%d r4=%d", rssi3, rssi4);
    FONTS_FillData(ZJ0_91in.data_buffer, &canvasInfo, 0, 16, str, &Font6x8_v, &fontDrawInfo);

    ssd1306_draw(&ZJ0_91in);
    LOG_I("draw finished.");
  }
}
