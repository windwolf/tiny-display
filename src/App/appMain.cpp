#include "base.hpp"

#include "display/ssd1306.hpp"
#include "fonts/fonts.hpp"
#include "message-parser.hpp"
#include "pin.hpp"
#include "uart.hpp"
#include "i2c.hpp"
#include "logger.hpp"
#include "app-framework.hpp"

#include "main.h"
#include "config.h"
#include "string.h"
#include "usart.h"
#include "spi.h"
#include "gpio.h"

using namespace wibot;

LOGGER("appmain")
class App : AppFramework {
   public:
    App();
    void OnSetup() override;
    void OnRun() override;

   private:
    FontDrawInfo fontDrawInfo;
    CanvasInfo   canvasInfo;
    EventGroup   eg;
    Pin          scl;
    Pin          sda;
    I2cMaster    softi2c;
    SSD1306      ZJ0_91in;

    uint8_t                 uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
    CircularBuffer<uint8_t> uartRxRingBuffer;
    UART                    uart1Dev;

    MessageParser                  mp;
    static constexpr MessageSchema schema = {
        .prefix     = {0xFF, 0xFE},
        .prefixSize = 2,

        .commandSize = MESSAGE_SCHEMA_SIZE::NONE,
        .defaultLength{
            .mode  = MESSAGE_LENGTH_SCHEMA_MODE::FIXED_LENGTH,
            .fixed = {.length = 14},
        },

        .alterDataSize = MESSAGE_SCHEMA_SIZE::NONE,

        .crcSize    = MESSAGE_SCHEMA_SIZE::NONE,
        .suffixSize = 0,
    };
    WaitHandler wh;
};

App::App()
    : AppFramework(APP_NAME, APP_VERSION),
      eg(""),
      scl(*GPIOB, GPIO_PIN_6),
      sda(*GPIOB, GPIO_PIN_7),
      softi2c(scl, sda),
      ZJ0_91in(softi2c),
      uartRxRingBuffer(uart1_rx_buffer, UART1_RX_BUFFER_SIZE),
      uart1Dev(huart1, "app"),
      mp(schema, uartRxRingBuffer),
      wh(){};
void App::OnSetup() {
    scl.config.inverse = false;
    sda.config.inverse = false;
    ZJ0_91in.config    = SSD1306Config{
           .width              = 128,
           .height             = 32,
           .memoryMode         = SSD1306_MEMORY_ADDRESSING_MODE_VERTICAL,
           .enableChargePump   = true,
           .comInverted        = true,
           .segmentInverted    = true,
           .comLeftRightRemap  = false,
           .comAlternative     = false,
           .displayStartLine   = 32,
           .displayOffset      = 0,
           .multiplexRatio     = 63,
           .displayInverted    = false,
           .phase1period       = 0x02,
           .phase2period       = 0x02,
           .vcomhDeselectLevel = SSD1306_VCOMH_DESELECT_LEVEL_VCC083,
           .fosc               = 0x0F,
           .clkDivide          = 0x00,
    };
    fontDrawInfo = {
        .foreColor =
            {
                .value = 0x1,
            },
        .backColor =
            {
                .value = 0x0,
            },
        .spacing     = 0,
        .lineSpacing = 1,
    };
    canvasInfo = {
        .width     = 128,
        .height    = 32,
        .pixelSize = PixelSize::Bit1,
        .direction = CanvasMemoryLayoutDirection::Vertical,

    };
    ZJ0_91in.lcd_init();
    // message_parser_test();
    // wibot::device::demo::st77xx_demo();
    // wibot::device::demo::w25qxx_demo();
    uart1Dev.start(uartRxRingBuffer, wh);
};

void App::OnRun() {
    LOG_I("loop begin");
    uint16_t rssi;
    uint16_t rssi1;
    uint16_t rssi2;
    uint16_t rssi3;
    uint16_t rssi4;
    bool     get = false;

    uint8_t      fData[16];
    MessageFrame frame({fData, 16});
    wh.wait(TIMEOUT_FOREVER);

    while (mp.parse(&frame) == Result::kOk) {
        auto ctn = frame.getContent();
        rssi     = ctn.GetUint16(0, false);
        rssi1    = ctn.GetUint16(2, false);
        rssi2    = ctn.GetUint16(4, false);
        rssi3    = ctn.GetUint16(6, false);
        rssi4    = ctn.GetUint16(8, false);
        get      = true;
    };

    if (get) {
        char str[22] = {0};
        ZJ0_91in.clear();

        sprintf(str, "rssi=%d", rssi);
        FONTS_FillData(ZJ0_91in.dataBuffer, &canvasInfo, 0, 0, str, &Font6x8_v, &fontDrawInfo);

        sprintf(str, "r1=%d r2=%d", rssi1, rssi2);
        FONTS_FillData(ZJ0_91in.dataBuffer, &canvasInfo, 0, 8, str, &Font6x8_v, &fontDrawInfo);

        sprintf(str, "r3=%d r4=%d", rssi3, rssi4);
        FONTS_FillData(ZJ0_91in.dataBuffer, &canvasInfo, 0, 16, str, &Font6x8_v, &fontDrawInfo);
        ZJ0_91in.draw();
        LOG_I("draw finished.");
    }
};
static App app1 = App();
