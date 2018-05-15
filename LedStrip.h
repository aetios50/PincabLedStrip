#ifndef LedStrip_h
#define LedStrip_h

#include <Arduino.h>

#ifndef DEBUG_ON_WIFI
#define FASTLED_ALLOW_INTERRUPTS 0
#endif

#include <FastLED.h>
FASTLED_USING_NAMESPACE


#define DATA_PIN1   12 // GPIO12 / D7 on WeMos/NodeMCU ESP8266
#define DATA_PIN2   13 // D6 
#define DATA_PIN3   14 // D5

#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
//#define COLOR_ORDER GRB

/*************   VALUE TO CHANGE  *******************/
//Defines the max number of ledstrip which is allowed per ledstriptrip.
#define MaxLedsPerStrip 432
//Defines the number of ledstrip
#define NUMBER_LEDSTRIP 1
//Defines Brightness
#define BRIGHTNESS  255
/*************   END VALUE TO CHANGE  *******************/

class LedStrip {
  public:
    LedStrip(uint32_t numPerStrip);
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
};

#endif
