#ifndef NATIVE_STUB_H
#define NATIVE_STUB_H

#ifdef NATIVE_BUILD

/* Native build stubs for hardware dependencies - minimal set */

/* Basic types */
typedef void* EventGroupHandle_t;
typedef void* SemaphoreHandle_t;
typedef void* TaskHandle_t;
typedef void* xQueueHandle_t;
typedef void* StaticEventGroup_t;
typedef void* StaticSemaphore_t;
typedef void* StaticTask_t;
typedef void* StaticQueue_t;
typedef void* StaticTimer_t;
typedef void* StaticMutex_t;
typedef void* StaticBinarySemaphore_t;
typedef void* StaticCountingSemaphore_t;

typedef uint32_t BaseType_t;
typedef uint32_t UBaseType_t;
typedef uint32_t TickType_t;
typedef uint32_t EventBits_t;
typedef void (*TaskFunction_t)(void*);
typedef uint8_t StackType_t;

/* Mock constants */
#define pdTRUE 1
#define pdFALSE 0
#define portYIELD_FROM_ISR(x) do {} while(0)
#define configASSERT(x) do {} while(0)
#define _BV(x) (1 << (x))

/* Mock function declarations */
static inline EventGroupHandle_t xEventGroupCreate(void) {
    return (EventGroupHandle_t)0x12345678;
}

static inline void vTaskDelay(TickType_t xTicksToDelay) {
    (void)xTicksToDelay;
}

/* Mock I2C functions */
static inline void i2c_begin(void) {}
static inline void i2c_end(void) {}
static inline int i2c_start(void) { return 0; }
static inline void i2c_stop(void) {}
static inline void i2c_write(int data) {(void)data;}
static inline int i2c_read(int ack) { (void)ack; return 0; }

/* Mock display functions */
static inline void tft_init(void) {}
static inline void tft_fillScreen(int color) {(void)color;}
static inline void tft_drawPixel(int x, int y, int color) {(void)x;(void)y;(void)color;}
static inline void tft_drawLine(int x0, int y0, int x1, int y1, int color) {
    (void)x0;(void)y0;(void)x1;(void)y1;(void)color;
}
static inline void tft_drawRect(int x, int y, int w, int h, int color) {
    (void)x;(void)y;(void)w;(void)h;(void)color;
}
static inline void tft_fillRect(int x, int y, int w, int h, int color) {
    (void)x;(void)y;(void)w;(void)h;(void)color;
}
static inline void tft_drawCircle(int x, int y, int r, int color) {
    (void)x;(void)y;(void)r;(void)color;
}
static inline void tft_fillCircle(int x, int y, int r, int color) {
    (void)x;(void)y;(void)r;(void)color;
}
static inline void tft_drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color) {
    (void)x0;(void)y0;(void)x1;(void)y1;(void)x2;(void)y2;(void)color;
}
static inline void tft_fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color) {
    (void)x0;(void)y0;(void)x1;(void)y1;(void)x2;(void)y2;(void)color;
}
static inline void tft_drawRoundRect(int x, int y, int w, int h, int r, int color) {
    (void)x;(void)y;(void)w;(void)h;(void)r;(void)color;
}
static inline void tft_fillRoundRect(int x, int y, int w, int h, int r, int color) {
    (void)x;(void)y;(void)w;(void)h;(void)r;(void)color;
}
static inline void tft_drawBitmap(int x, int y, const unsigned char *bitmap, int w, int h, int color) {
    (void)x;(void)y;(void)bitmap;(void)w;(void)h;(void)color;
}
static inline void tft_drawChar(int x, int y, char c, int color, int bg, int size) {
    (void)x;(void)y;(void)c;(void)color;(void)bg;(void)size;
}
static inline void tft_setCursor(int x, int y) {(void)x;(void)y;}
static inline void tft_setTextColor(int color) {(void)color;}
static inline void tft_setTextSize(int size) {(void)size;}
static inline void tft_print(char *str) {(void)str;}
static inline void tft_println(char *str) {(void)str;}
static inline void tft_clearDisplay(void) {}
static inline void tft_displayOn(void) {}
static inline void tft_displayOff(void) {}
static inline int tft_width(void) { return 240; }
static inline int tft_height(void) { return 240; }
static inline int tft_rotation(void) { return 0; }
static inline void tft_setRotation(int r) {(void)r;}

/* Mock touch functions */
static inline int touch_getX(void) { return 0; }
static inline int touch_getY(void) { return 0; }
static inline int touch_isPressed(void) { return 0; }

/* Mock power functions */
static inline int power_getBatteryLevel(void) { return 100; }
static inline float power_getBatteryVoltage(void) { return 4.2; }
static inline int power_isCharging(void) { return 0; }

/* Mock sensor functions */
static inline int sensor_getX(void) { return 0; }
static inline int sensor_getY(void) { return 0; }
static inline int sensor_getZ(void) { return 0; }

/* Mock LVGL functions */
static inline void lv_init(void) {}
static inline void lv_disp_flush_ready(void *disp_drv) {(void)disp_drv;}
static inline void lv_indev_read_ready(void *indev_drv) {(void)indev_drv;}

/* Mock time functions */
static inline void rtc_begin(void) {}
static inline int rtc_getHour(void) { return 0; }
static inline int rtc_getMinute(void) { return 0; }
static inline int rtc_getSecond(void) { return 0; }
static inline int rtc_getDay(void) { return 1; }
static inline int rtc_getMonth(void) { return 1; }
static inline int rtc_getYear(void) { return 2024; }

/* Mock AXP202 functions */
static inline int axp_begin(void *i2c, void *read, void *write) {
    (void)i2c;(void)read;(void)write;
    return 0;
}
static inline void axp_setShutdownTime(int time) {(void)time;}
static inline void axp_setChgLEDMode(int mode) {(void)mode;}
static inline void axp_setPowerOutPut(int pin, int state) {(void)pin;(void)state;}
static inline void axp_setLDO3Mode(int mode) {(void)mode;}
static inline void axp_setLDO3Voltage(int voltage) {(void)voltage;}
static inline void axp_setLDO2Voltage(int voltage) {(void)voltage;}
static inline void axp_setLDO4Voltage(int voltage) {(void)voltage;}
static inline void axp_limitingOff(void) {}
static inline void axp_setChargeControlCur(int cur) {(void)cur;}

/* Mock Button functions */
static inline void button_begin(void) {}
static inline int button_isPressed(void) { return 0; }
static inline int button_isReleased(void) { return 1; }

/* Mock BMA423 functions */
static inline int bma_begin(void *i2c) {(void)i2c; return 0; }
static inline int bma_setPowerMode(int mode) {(void)mode; return 0; }
static inline int bma_setOdr(int odr) {(void)odr; return 0; }
static inline int bma_setRange(int range) {(void)range; return 0; }

/* Mock MPU6050 functions */
static inline int mpu_begin(void) { return 0; }
static inline int mpu_testConnection(void) { return 1; }
static inline int mpu_getX(void) { return 0; }
static inline int mpu_getY(void) { return 0; }
static inline int mpu_getZ(void) { return 0; }

/* Mock I2C Bus functions */
static inline int i2c_bus_begin(void) { return 0; }
static inline int i2c_bus_end(void) { return 0; }
static inline int i2c_bus_writeBytes(int devAddress, int regAddress, unsigned char *data, int len) {
    (void)devAddress;(void)regAddress;(void)data;(void)len;
    return 0;
}
static inline int i2c_bus_readBytes(int devAddress, int regAddress, unsigned char *data, int len) {
    (void)devAddress;(void)regAddress;(void)data;(void)len;
    return 0;
}
static inline int i2c_bus_writeBytes_u16(int devAddress, int regAddress, unsigned char *data, int len) {
    (void)devAddress;(void)regAddress;(void)data;(void)len;
    return 0;
}
static inline int i2c_bus_readBytes_u16(int devAddress, int regAddress, unsigned char *data, int len) {
    (void)devAddress;(void)regAddress;(void)data;(void)len;
    return 0;
}
static inline int i2c_bus_scan(void) { return 0; }

/* Mock TFT_eSPI functions */
static inline void tft_eSPI_init(void) {}
static inline void tft_eSPI_fillScreen(int color) {(void)color;}
static inline void tft_eSPI_drawPixel(int x, int y, int color) {(void)x;(void)y;(void)color;}
static inline void tft_eSPI_drawLine(int x0, int y0, int x1, int y1, int color) {
    (void)x0;(void)y0;(void)x1;(void)y1;(void)color;
}
static inline void tft_eSPI_drawRect(int x, int y, int w, int h, int color) {
    (void)x;(void)y;(void)w;(void)h;(void)color;
}
static inline void tft_eSPI_fillRect(int x, int y, int w, int h, int color) {
    (void)x;(void)y;(void)w;(void)h;(void)color;
}
static inline void tft_eSPI_drawCircle(int x, int y, int r, int color) {
    (void)x;(void)y;(void)r;(void)color;
}
static inline void tft_eSPI_fillCircle(int x, int y, int r, int color) {
    (void)x;(void)y;(void)r;(void)color;
}
static inline void tft_eSPI_drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color) {
    (void)x0;(void)y0;(void)x1;(void)y1;(void)x2;(void)y2;(void)color;
}
static inline void tft_eSPI_fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color) {
    (void)x0;(void)y0;(void)x1;(void)y1;(void)x2;(void)y2;(void)color;
}
static inline void tft_eSPI_drawRoundRect(int x, int y, int w, int h, int r, int color) {
    (void)x;(void)y;(void)w;(void)h;(void)r;(void)color;
}
static inline void tft_eSPI_fillRoundRect(int x, int y, int w, int h, int r, int color) {
    (void)x;(void)y;(void)w;(void)h;(void)r;(void)color;
}
static inline void tft_eSPI_drawBitmap(int x, int y, const unsigned char *bitmap, int w, int h, int color) {
    (void)x;(void)y;(void)bitmap;(void)w;(void)h;(void)color;
}
static inline void tft_eSPI_drawChar(int x, int y, char c, int color, int bg, int size) {
    (void)x;(void)y;(void)c;(void)color;(void)bg;(void)size;
}
static inline void tft_eSPI_setCursor(int x, int y) {(void)x;(void)y;}
static inline void tft_eSPI_setTextColor(int color) {(void)color;}
static inline void tft_eSPI_setTextSize(int size) {(void)size;}
static inline void tft_eSPI_print(char *str) {(void)str;}
static inline void tft_eSPI_println(char *str) {(void)str;}
static inline void tft_eSPI_clearDisplay(void) {}
static inline void tft_eSPI_displayOn(void) {}
static inline void tft_eSPI_displayOff(void) {}
static inline int tft_eSPI_width(void) { return 240; }
static inline int tft_eSPI_height(void) { return 240; }
static inline int tft_eSPI_rotation(void) { return 0; }
static inline void tft_eSPI_setRotation(int r) {(void)r;}
static inline void tft_eSPI_startWrite(void) {}
static inline void tft_eSPI_endWrite(void) {}
static inline void tft_eSPI_setAddrWindow(int x, int y, int w, int h) {
    (void)x;(void)y;(void)w;(void)h;
}
static inline void tft_eSPI_pushColors(unsigned short *data, int len, int first) {
    (void)data;(void)len;(void)first;
}
static inline void tft_eSPI_pushPixelsDMA(unsigned short *data, int len) {
    (void)data;(void)len;
}

/* Mock U8g2 functions */
static inline void u8g2_begin(void) {}
static inline void u8g2_clearDisplay(void) {}
static inline void u8g2_drawPixel(int x, int y) {(void)x;(void)y;}
static inline void u8g2_drawLine(int x0, int y0, int x1, int y1) {
    (void)x0;(void)y0;(void)x1;(void)y1;
}
static inline void u8g2_drawRectangle(int x, int y, int w, int h) {
    (void)x;(void)y;(void)w;(void)h;
}
static inline void u8g2_drawCircle(int x, int y, int radius) {
    (void)x;(void)y;(void)radius;
}
static inline void u8g2_drawArc(int x, int y, int radius, int startAngle, int endAngle) {
    (void)x;(void)y;(void)radius;(void)startAngle;(void)endAngle;
}
static inline void u8g2_drawBox(int x, int y, int w, int h) {
    (void)x;(void)y;(void)w;(void)h;
}
static inline void u8g2_drawRoundedBox(int x, int y, int w, int h, int r) {
    (void)x;(void)y;(void)w;(void)h;(void)r;
}
static inline void u8g2_drawRoundedRectangle(int x, int y, int w, int h, int r) {
    (void)x;(void)y;(void)w;(void)h;(void)r;
}
static inline void u8g2_drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
    (void)x0;(void)y0;(void)x1;(void)y1;(void)x2;(void)y2;
}
static inline void u8g2_drawPolygon(int *points, int nPoints) {
    (void)points;(void)nPoints;
}
static inline void u8g2_drawHLine(int x, int y, int w) {(void)x;(void)y;(void)w;}
static inline void u8g2_drawVLine(int x, int y, int h) {(void)x;(void)y;(void)h;}
static inline void u8g2_drawFrame(int x, int y, int w, int h) {
    (void)x;(void)y;(void)w;(void)h;
}
static inline void u8g2_drawFrameRounded(int x, int y, int w, int h, int r) {
    (void)x;(void)y;(void)w;(void)h;(void)r;
}
static inline void u8g2_drawBoxRounded(int x, int y, int w, int h, int r) {
    (void)x;(void)y;(void)w;(void)h;(void)r;
}
static inline void u8g2_drawEllipseFrame(int x, int y, int rx, int ry) {
    (void)x;(void)y;(void)rx;(void)ry;
}
static inline void u8g2_drawEllipse(int x, int y, int rx, int ry) {
    (void)x;(void)y;(void)rx;(void)ry;
}
static inline void u8g2_drawCircleFrame(int x, int y, int radius) {
    (void)x;(void)y;(void)radius;
}
static inline void u8g2_drawDisc(int x, int y, int radius) {
    (void)x;(void)y;(void)radius;
}
static inline void u8g2_drawPie(int x, int y, int radius, int startAngle, int endAngle) {
    (void)x;(void)y;(void)radius;(void)startAngle;(void)endAngle;
}
static inline void u8g2_drawRPolygon(int *points, int nPoints, int r) {
    (void)points;(void)nPoints;(void)r;
}
static inline void u8g2_drawStr(int x, int y, const char *s) {
    (void)x;(void)y;(void)s;
}
static inline void u8g2_drawUTF8(int x, int y, const char *s) {
    (void)x;(void)y;(void)s;
}
static inline void u8g2_drawGlyph(int x, int y, int glyph) {
    (void)x;(void)y;(void)glyph;
}
static inline void u8g2_drawXBM(int x, int y, int w, int h, const unsigned char *xbm) {
    (void)x;(void)y;(void)w;(void)h;(void)xbm;
}
static inline void u8g2_setClipWindow(int x, int y, int w, int h) {
    (void)x;(void)y;(void)w;(void)h;
}
static inline void u8g2_enableClipWindow(void) {}
static inline void u8g2_disableClipWindow(void) {}
static inline void u8g2_setFontMode(int mode) {(void)mode;}
static inline void u8g2_setFontRefHeightText(void) {}
static inline void u8g2_setFontRefHeightExtendedText(void) {}
static inline void u8g2_setFontPosBaseline(void) {}
static inline void u8g2_setFontPosTop(void) {}
static inline void u8g2_setFontPosBottom(void) {}
static inline void u8g2_setFontPosCenter(void) {}
static inline void u8g2_setDrawColor(int color) {(void)color;}
static inline void u8g2_setXBMColor(int color) {(void)color;}
static inline void u8g2_setContrast(int value) {(void)value;}
static inline void u8g2_clearContrast(void) {}
static inline void u8g2_enableReverse(void) {}
static inline void u8g2_disableReverse(void) {}
static inline void u8g2_enableFastX(void) {}
static inline void u8g2_disableFastX(void) {}
static inline void u8g2_enableFastY(void) {}
static inline void u8g2_disableFastY(void) {}
static inline void u8g2_enableFlip(void) {}
static inline void u8g2_disableFlip(void) {}
static inline void u8g2_enableUTF8Print(void) {}
static inline void u8g2_disableUTF8Print(void) {}
static inline void u8g2_enableMenuBox(void) {}
static inline void u8g2_disableMenuBox(void) {}
static inline void u8g2_enableMenuBoxTitle(void) {}
static inline void u8g2_disableMenuBoxTitle(void) {}
static inline void u8g2_enableMenuBoxItems(void) {}
static inline void u8g2_disableMenuBoxItems(void) {}
static inline void u8g2_enableMenuBoxFrame(void) {}
static inline void u8g2_disableMenuBoxFrame(void) {}
static inline void u8g2_enableMenuBoxBorder(void) {}
static inline void u8g2_disableMenuBoxBorder(void) {}
static inline void u8g2_enableMenuBoxHelp(void) {}
static inline void u8g2_disableMenuBoxHelp(void) {}
static inline void u8g2_enableMenuBoxCursor(void) {}
static inline void u8g2_disableMenuBoxCursor(void) {}
static inline void u8g2_enableMenuBoxCursorFrame(void) {}
static inline void u8g2_disableMenuBoxCursorFrame(void) {}
static inline void u8g2_enableMenuBoxCursorBorder(void) {}
static inline void u8g2_disableMenuBoxCursorBorder(void) {}
static inline void u8g2_enableMenuBoxCursorHelp(void) {}
static inline void u8g2_disableMenuBoxCursorHelp(void) {}
static inline void u8g2_enableMenuBoxCursorTitle(void) {}
static inline void u8g2_disableMenuBoxCursorTitle(void) {}
static inline void u8g2_enableMenuBoxCursorItems(void) {}
static inline void u8g2_disableMenuBoxCursorItems(void) {}
static inline void u8g2_enableMenuBoxCursorFrameBorder(void) {}
static inline void u8g2_disableMenuBoxCursorFrameBorder(void) {}
static inline void u8g2_enableMenuBoxCursorFrameHelp(void) {}
static inline void u8g2_disableMenuBoxCursorFrameHelp(void) {}
static inline void u8g2_enableMenuBoxCursorFrameTitle(void) {}
static inline void u8g2_disableMenuBoxCursorFrameTitle(void) {}
static inline void u8g2_enableMenuBoxCursorFrameItems(void) {}
static inline void u8g2_disableMenuBoxCursorFrameItems(void) {}
static inline void u8g2_enableMenuBoxCursorBorderHelp(void) {}
static inline void u8g2_disableMenuBoxCursorBorderHelp(void) {}
static inline void u8g2_enableMenuBoxCursorBorderTitle(void) {}
static inline void u8g2_disableMenuBoxCursorBorderTitle(void) {}
static inline void u8g2_enableMenuBoxCursorBorderItems(void) {}
static inline void u8g2_disableMenuBoxCursorBorderItems(void) {}
static inline void u8g2_enableMenuBoxCursorHelpTitle(void) {}
static inline void u8g2_disableMenuBoxCursorHelpTitle(void) {}
static inline void u8g2_enableMenuBoxCursorHelpItems(void) {}
static inline void u8g2_disableMenuBoxCursorHelpItems(void) {}
static inline void u8g2_enableMenuBoxCursorTitleItems(void) {}
static inline void u8g2_disableMenuBoxCursorTitleItems(void) {}
static inline void u8g2_enableMenuBoxCursorHelpTitleItems(void) {}
static inline void u8g2_disableMenuBoxCursorHelpTitleItems(void) {}

#endif /* NATIVE_BUILD */

#endif /* NATIVE_STUB_H */
