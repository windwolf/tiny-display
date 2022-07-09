#include "ssd1306_demo.h"
#include "common/device.h"
#include "device/ssd1306.h"
#include "bsp.h"
#include "fonts/fonts.h"
#include "stm32f1xx_hal.h"

#define LOG_MODULE "SSD1306demo"
#include "log.h"

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

static void device_init()
{
    pin_device_create(&scl, GPIOB, GPIO_PIN_6, PIN_DEVICE_STATUS_INVERSE_NORMAL);
    pin_device_create(&sda, GPIOB, GPIO_PIN_7, PIN_DEVICE_STATUS_INVERSE_NORMAL);
    softi2c.scl = &scl;
    softi2c.sda = &sda;
    i2c_device_create(&i2c, &softi2c);
    ssd1306_create(&ZJ0_91in, &i2c);
    ssd1306_init(&ZJ0_91in);
}
void ssd1306_demo(void)
{
    device_init();

    while (1)
    {
        FONTS_FillData(ZJ0_91in.data_buffer, &canvasInfo, 0, 0, "!!", &Font6x8_v, &fontDrawInfo);
        FONTS_FillData(ZJ0_91in.data_buffer, &canvasInfo, 0, 8, "??", &Font6x8_v, &fontDrawInfo);
        FONTS_FillData(ZJ0_91in.data_buffer, &canvasInfo, 0, 16, "AA", &Font6x8_v, &fontDrawInfo);
        FONTS_FillData(ZJ0_91in.data_buffer, &canvasInfo, 0, 24, "aa", &Font6x8_v, &fontDrawInfo);
        ssd1306_draw(&ZJ0_91in);
        LOG_I("draw finished.");
    }
}