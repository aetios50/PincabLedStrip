#ifndef LedStrip_h
#define LedStrip_h

#include <Arduino.h>

#ifndef DEBUG_ON_WIFI
#define FASTLED_ALLOW_INTERRUPTS 0
#endif

#include <FastLED.h>
FASTLED_USING_NAMESPACE


#define DATA_PIN1   14 // D5
#define DATA_PIN2   12 // D6 
#define DATA_PIN3   13 // D7
#define DATA_PIN4   15 // D8
#define DATA_PIN5   5 // D1 
#define DATA_PIN6   2 // D4

//#define LED_TYPE    WS2811
#define LED_TYPE    WS2812
//#define COLOR_ORDER RGB
#define COLOR_ORDER GRB

/*************   VALUE TO CHANGE  *******************/
//Defines the max number of ledstrip which is allowed per ledstrip.
#define MaxLedsPerStrip 1024
//Defines the number of ledstrip
#define NUMBER_LEDSTRIP 6



//Defines Brightness
#define BRIGHTNESS 16 // Defines Brightness in RGB boot sequence
#define BRIGHTNESS_PIN1 255 // Defines Brightness in pin 1 (0 to 255)
#define BRIGHTNESS_PIN2 255 // Defines Brightness in pin 2 (0 to 255)
#define BRIGHTNESS_PIN3 255 // Defines Brightness in pin 3 (0 to 255)
#define BRIGHTNESS_PIN4 255 // Defines Brightness in pin 4 (0 to 255)
#define BRIGHTNESS_PIN5 255 // Defines Brightness in pin 5 (0 to 255)
#define BRIGHTNESS_PIN6 255 // Defines Brightness in pin 6 (0 to 255)
/*************   END VALUE TO CHANGE  *******************/

class LedStrip {
  public:
    LedStrip(uint32_t numPerStrip);
    void begin(void);

    void addNewStrip(uint8_t index,uint16_t length);
    void setStripLength(uint16_t length);
    void clearAll();
    void setPixels(uint32_t start_num,uint16_t len,int color);
    void setPixel(uint32_t num, int color);
    void setPixel(uint32_t num, uint8_t red, uint8_t green, uint8_t blue) {
      setPixel(num, color(red, green, blue));
    }
    int getPixel(uint32_t num);

    void show(void);
    int busy(void);

    int numPixels(void) {
      return stripTotalLen * 8;
    }
    int color(uint8_t red, uint8_t green, uint8_t blue) {
      return (red << 16) | (green << 8) | blue;
    }


  private:
    static uint16_t stripTotalLen;
    static uint16_t stripLen[NUMBER_LEDSTRIP];
    static uint16_t stripStartOffset[NUMBER_LEDSTRIP];
    static uint8_t stripIndex[NUMBER_LEDSTRIP*MaxLedsPerStrip];

};

#endif
