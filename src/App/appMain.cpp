#include "base.hpp"

#include "display/ssd1306.hpp"
#include "fonts/fonts.hpp"
#include "message-parser.hpp"
#include "pin.hpp"
#include "logger.hpp"
#include "app-framework.hpp"

#include "main.h"
#include "config.h"
#include "string.h"
#include "usart.h"
#include "spi.h"
#include "gpio.h"
#include "stream.hpp"
#include "soft-i2c.hpp"

using namespace wibot;

LOGGER("appmain")

class App : AppFramework {
   public:
    App()
        : AppFramework(APP_NAME, APP_VERSION),
          softi2c(GPIOB, GPIO_PIN_6, GPIOB, GPIO_PIN_7),
          ZJ0_91in(&softi2c),
          uart1Dev(&huart1, "app"),
          stream(uart1Dev, uartBuffer, schema){};
    void onSetup() override {
        ZJ0_91in.config = Ssd1306Config{
            .width              = 128,
            .height             = 32,
            .memoryMode         = kVERTICAL,
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
            .vcomhDeselectLevel = kVCC083,
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
        ZJ0_91in.init();
        stream.open();
    };
    void onRun() override {
        LOG_I("loop begin");

        uint8_t      fData[16];
        MessageFrame frame({fData, 16});

        while (true) {
            auto rst = stream.readSync(frame, TIMEOUT_FOREVER);
            if (rst == Result::kOk) {
                auto     ctn     = frame.getContent();
                uint16_t rssi    = ctn.getUint16(0);
                uint16_t rssi1   = ctn.getUint16(2);
                uint16_t rssi2   = ctn.getUint16(4);
                uint16_t rssi3   = ctn.getUint16(6);
                uint16_t rssi4   = ctn.getUint16(8);
                char     str[22] = {0};
                ZJ0_91in.clear();

                sprintf(str, "rssi=%d", rssi);
                FONTS_FillData(ZJ0_91in.dataBuffer, &canvasInfo, 0, 0, str, &Font6x8_v,
                               &fontDrawInfo);

                sprintf(str, "r1=%d r2=%d", rssi1, rssi2);
                FONTS_FillData(ZJ0_91in.dataBuffer, &canvasInfo, 0, 8, str, &Font6x8_v,
                               &fontDrawInfo);

                sprintf(str, "r3=%d r4=%d", rssi3, rssi4);
                FONTS_FillData(ZJ0_91in.dataBuffer, &canvasInfo, 0, 16, str, &Font6x8_v,
                               &fontDrawInfo);
                ZJ0_91in.draw();
                LOG_I("draw finished.");
            }
        }
    };

   private:
    FontDrawInfo  fontDrawInfo;
    CanvasInfo    canvasInfo;
    SoftI2cMaster softi2c;
    Ssd1306       ZJ0_91in;

    BUFFER(uartBuffer, 32);
    Uart uart1Dev;

    static constexpr MessageSchema schema = {
        .prefix     = {0xFF, 0xFE},
        .prefixSize = 2,

        .commandSize = DataWidth::kNone,
        .defaultLength{
            .mode  = MessageLengthSchemaMode::kFixedLength,
            .fixed = {.length = 14},
        },

        .alterDataSize = DataWidth::kNone,

        .crcSize    = DataWidth::kNone,
        .suffixSize = 0,
    };
    Stream stream;
};
static App app1 = App();
