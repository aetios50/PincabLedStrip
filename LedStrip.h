#ifndef LedStrip_h
#define LedStrip_h

/***/
//#define DEBUG_ON_WIFI
//#ifdef DEBUG_ON_WIFI
//#include <WifiDebug.h>
//static WifiDebug wifidebug;
//#endif
/***/

#include <Arduino.h>
/*
#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0
*/
#include <FastLED.h>
FASTLED_USING_NAMESPACE

class LedStrip {
public:
LedStrip(uint32_t numPerStrip,CRGB *leds);
void begin(void);

void setStripLength(uint16_t length);
void setPixel(uint32_t num, int color);
void setPixel(uint32_t num, uint8_t red, uint8_t green, uint8_t blue) {
								setPixel(num, color(red, green, blue));
}
int getPixel(uint32_t num);

void show(void);
int busy(void);

int numPixels(void) {
								return stripLen * 8;
}
int color(uint8_t red, uint8_t green, uint8_t blue) {
								return (red << 16) | (green << 8) | blue;
}


private:
static uint16_t stripLen;
static void *frameBuffer;
static void *drawBuffer;
static uint8_t params;
static void isr(void);
};

#endif
