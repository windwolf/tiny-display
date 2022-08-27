#include "appMain.hpp"
#include "accessor/uart_stream.hpp"
#include "device/ssd1306.hpp"
#include "fonts/fonts.hpp"
#include "message/message_parser.hpp"

#include "ringbuffer.hpp"
#include "soc.hpp"
#include "string.h"
#include "wait_handler.hpp"

//#include "st77xx_demo.hpp"
//#include "w25qxx_demo.hpp"
#include "message/message_parser_test.hpp"

#define LOG_MODULE "appMain"
#include "log.h"

extern UART_HandleTypeDef huart1;
SPI_HandleTypeDef hspi1;
namespace app
{

using namespace ww::peripheral;
using namespace ww::accessor;
using namespace ww::device;
using namespace ww;
using namespace ww::os;
using namespace ww::graph;
using namespace ww::comm;
using namespace ww::comm::test;

class App
{

  public:
    App();
    void setup();
    void loop();

  private:
    FontDrawInfo fontDrawInfo;
    CanvasInfo canvasInfo;

    Pin scl;
    Pin sda;
    I2cMaster softi2c;
    SSD1306 ZJ0_91in;

    uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
    RingBuffer uartRxRingBuffer;
    UART uart1Dev;

    UartStream stream;
    MessageParser mp;
    MessageSchema schema;
    PollingWaitHandler wh;
};

App::App()
    : scl(*GPIOB, GPIO_PIN_6), sda(*GPIOB, GPIO_PIN_7), softi2c(scl, sda),
      ZJ0_91in(softi2c),
      uartRxRingBuffer(uart1_rx_buffer, 1, UART1_RX_BUFFER_SIZE),
      uart1Dev(huart1), stream(uart1Dev, uartRxRingBuffer),
      mp(uartRxRingBuffer), wh(){

                            };
void App::setup()
{
    scl.config_get().inverse = false;
    scl.init();
    sda.config_get().inverse = false;
    sda.init();
    softi2c.init();
    auto &sd3306_cfg = ZJ0_91in.config_get();
    sd3306_cfg = SSD1306Config{
        .width = 128,
        .height = 32,
        .memoryMode = SSD1306_MEMORY_ADDRESSING_MODE_VERTICAL,
        .enableChargePump = true,
        .comInverted = true,
        .segmentInverted = true,
        .comLeftRightRemap = false,
        .comAlternative = false,
        .displayStartLine = 32,
        .displayOffset = 0,
        .multiplexRatio = 63,
        .displayInverted = false,
        .phase1period = 0x02,
        .phase2period = 0x02,
        .vcomhDeselectLevel = SSD1306_VCOMH_DESELECT_LEVEL_VCC083,
        .fosc = 0x0F,
        .clkDivide = 0x00,
    };
    ZJ0_91in.init();
    uart1Dev.init();
    stream.init();
    mp.init(MessageSchema{
        .prefix = {0xFF, 0xFE},
        .prefixSize = 2,
        .mode = MESSAGE_SCHEMA_MODE_FIXED_LENGTH,
        .cmdLength = MESSAGE_SCHEMA_SIZE_NONE,
        .fixed =
            {
                .length = 14,
            },
        .alterDataSize = MESSAGE_SCHEMA_SIZE_NONE,

        .crc =
            {
                .length = MESSAGE_SCHEMA_SIZE_NONE,
            },
        .suffixSize = 0,
    });

    fontDrawInfo = {
        .foreColor =
            {
                .value = 0x1,
            },
        .backColor =
            {
                .value = 0x0,
            },
        .spacing = 0,
        .lineSpacing = 1,
    };
    canvasInfo = {
        .width = 128,
        .height = 32,
        .pixelSize = PIXEL_SIZE_1BIT,
        .direction = CANVAS_MEMORY_LAYOUT_DIRECTION_VERTICAL,

    };
    ZJ0_91in.lcd_init();
    // message_parser_test();
    // ww::device::demo::st77xx_demo();
    // ww::device::demo::w25qxx_demo();
    stream.server_start(wh);
};

void App::loop()
{
    LOG_I("loop begin");
    uint16_t rssi;
    uint16_t rssi1;
    uint16_t rssi2;
    uint16_t rssi3;
    uint16_t rssi4;
    bool get = false;
    MessageFrame frame;
    uint8_t fData[16];

    wh.wait(TIMEOUT_FOREVER);
    wh.reset();
    while (mp.frame_get(nullptr, frame) == Result_OK)
    {
        frame.extract(fData);
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
};
} // namespace app

static app::App app1 = app::App();

void setup(void)
{
    app1.setup();
}

void loop(void)
{
    app1.loop();
}
