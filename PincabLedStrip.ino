/********************************************************************************************************
** Pincab Ledstrip Controller
********************************************************************************************************/
/*
 * This code is fully compatible with WEMOS D1 PRO
 */

#define SERIAL_BUFFER_SIZE 2048

#include <elapsedMillis.h>

/***/
//#define DEBUG_ON_WIFI
#ifdef DEBUG_ON_WIFI
#include "WifiDebug.h"
static WifiDebug wifidebug;
#endif
/***/

#include "LedStrip.h"

#define FirmwareVersionMajor 1
#define FirmwareVersionMinor 2

//Defines the Pinnumber to which the built in led
#define LedPin D4

//Variable used to control the blinking and flickering of the led of the Wemos
elapsedMillis BlinkTimer;
int BlinkMode;
elapsedMillis BlinkModeTimeoutTimer;

LedStrip ledstrip(MaxLedsPerStrip);

uint32_t configuredStripLength = MaxLedsPerStrip;

//Setup of the system. Is called once on startup.
void setup() {
  Serial.begin(2000000);//921600);
  while (Serial.available()) {
    Serial.read();
  };
  delay(100);
  Serial.println("");

  /**/
#ifdef DEBUG_ON_WIFI
  wifidebug.begin();
#endif
  /**/

  //Initialize the lib for the ledstrip
  ledstrip.setStripLength(configuredStripLength);
  ledstrip.begin();
  ledstrip.show();

  //Initialize the led pin
  pinMode(LedPin, OUTPUT);

  SetBlinkMode(0);

#ifdef DEBUG_ON_WIFI
  wifidebug.debug_send_msg("Setup done");
#endif

  /****/
  ClearAllLedData();
  ledstrip.show();
for (uint32_t i = 0; i < configuredStripLength* NUMBER_LEDSTRIP; i++) {
  ledstrip.setPixel(i, BRIGHTNESS, 0, 0);
  }
  ledstrip.show();
  FastLED.delay(200);
  ClearAllLedData();
  ledstrip.show();
  for (uint32_t i = 0; i < configuredStripLength* NUMBER_LEDSTRIP; i++) {
    ledstrip.setPixel(i, 0, BRIGHTNESS, 0);
  }
  ledstrip.show();
  FastLED.delay(200);
  ClearAllLedData();
  ledstrip.show();
  for (uint32_t i = 0; i < configuredStripLength* NUMBER_LEDSTRIP; i++) {
    ledstrip.setPixel(i, 0, 0, BRIGHTNESS);
  }
  ledstrip.show();
  FastLED.delay(200);
  ClearAllLedData();
  ledstrip.show();
  /**/
}

static byte receivedByte;

//Main loop of the programm gets called again and again.
void loop() {
  //Check if data is available
  if (Serial.available() > 0) {

    receivedByte = Serial.read();

    switch (receivedByte) {
      case 'Z':
        //Set length of a strip
        SetALedStripLength();
        break;
      case 'L':
        //Set length of strips
        SetLedStripLength();
        break;
      case 'F':
        //Fill strip area with color
        Fill();
        break;
      case 'R':
        //receive data for strips
        ReceiveData();
        break;
      case 'O':
        //output data on strip
        OutputData();
        break;
      case 'C':
        //Clears all previously received led data
        ClearAllLedData();
        break;
      case 'V':
        //Send the firmware version
        SendVersion();
        break;
      case 'M':
        //Get max number of ledstrip per strip
        SendMaxNumberOfLeds();
        break;
      default:
        // no unknown commands allowed. Send NACK (N)
        Nack();
        break;
    }
    SetBlinkMode(1);
  }
  Blink();
}

//Sets the mode for the blinking of the led
void SetBlinkMode(int Mode) {
  BlinkMode = Mode;
  BlinkModeTimeoutTimer = 0;
}

//Controls the blinking of the led
void Blink() {
  switch (BlinkMode) {
    case 0:
      //Blinkmode 0 is only active after the start of the Wemos until the first command is received.
      if (BlinkTimer < 1500) {
        digitalWrite(LedPin, 0);
      } else if (BlinkTimer < 1600) {
        digitalWrite(LedPin, 1);
      } else {
        BlinkTimer = 0;
        digitalWrite(LedPin, 0);
      }
      break;
    case 1:
      //Blinkmode 1 is activated when the Wemos receives a command
      //Mode expires 500ms after the last command has been received resp. mode has been set
      if (BlinkTimer > 30) {
        BlinkTimer = 0;
        digitalWrite(LedPin, !digitalRead(LedPin));
      }
      if (BlinkModeTimeoutTimer > 500) {
        SetBlinkMode(2);
      }
      break;
    case 2:
      //Blinkmode 2 is active while the Wemos is waiting for more commands
      if (BlinkTimer < 1500) {
        digitalWrite(LedPin, 0);
      } else if (BlinkTimer < 1600) {
        digitalWrite(LedPin, 1);
      } else if (BlinkTimer < 1700) {
        digitalWrite(LedPin, 0);
      } else if (BlinkTimer < 1800) {
        digitalWrite(LedPin, 1);
      } else {
        BlinkTimer = 0;
        digitalWrite(LedPin, 0);
      }
    default:
      //This should never be active
      //The code is only here to make it easier to determine if a wrong Blinkcode has been set
      if (BlinkTimer > 2000) {
        BlinkTimer = 0;
        digitalWrite(LedPin, !digitalRead(LedPin));
      }
      break;
  }
}


//Outputs the data in the ram to the ledstriptrips
void OutputData() {
  ledstrip.show();
  Ack();
}


//Fills the given area of a ledstriptrip with a color
void Fill() {
  word firstLed = ReceiveWord();
  word numberOfLeds = ReceiveWord();
  int ColorData = ReceiveColorData();
  if ( firstLed <= configuredStripLength * NUMBER_LEDSTRIP && numberOfLeds > 0 && firstLed + numberOfLeds - 1 <= configuredStripLength * NUMBER_LEDSTRIP ) {
    ledstrip.setPixels(firstLed,numberOfLeds,ColorData);
    /*word endLedNr = firstLed + numberOfLeds;
    for (word ledNr = firstLed; ledNr < endLedNr; ledNr++) {
      ledstrip.setPixel(ledNr, ColorData);
    }*/
    Ack();
  } else {
    //Number of the first led or the number of ledstrip to receive is outside the allowed range
    Nack();
  }
}


//Receives data for the ledstriptrips
void ReceiveData() {
  word firstLed = ReceiveWord();
  word numberOfLeds = ReceiveWord();
  if ( firstLed <= configuredStripLength * NUMBER_LEDSTRIP && numberOfLeds > 0 && firstLed + numberOfLeds - 1 <= configuredStripLength * NUMBER_LEDSTRIP ) {
    //FirstLedNr and numberOfLeds are valid.
    //Receive and set color data
    word endLedNr = firstLed + numberOfLeds;
    int i = 0;
    for (word ledNr = firstLed; ledNr < endLedNr; ledNr++) {
      i++;
      ledstrip.setPixel(ledNr, ReceiveColorData());
    }
    Ack();
  } else {
    //Number of the first led or the number of ledstrip to receive is outside the allowed range
    Nack();
  }
}

//Sets the length of the longest connected ledstriptrip. Length is restricted to the max number of allowed ledstrip
void SetLedStripLength() {
  word stripLength = ReceiveWord();
  if (stripLength < 1 || stripLength > MaxLedsPerStrip) {
    //stripLength is either to small or above the max number of ledstrip allowed
    Nack();
  } else {
    //stripLength is in the valid range
    configuredStripLength = stripLength;
    ledstrip.setStripLength(stripLength);
    Ack();
  }
}


//Sets the length of a led strip
void SetALedStripLength() {
  while (!Serial.available()) {};
  byte indexStrip = Serial.read();  while (!Serial.available()) {};
  byte lastStrip = Serial.read();
  while (!Serial.available()) {};
  word stripLength = ReceiveWord();
  /*if (stripLength < 1 || stripLength > MaxLedsPerStrip ) {
    //stripLength is either to small or above the max number of ledstrip allowed or strip index is not within existing range
    Nack();
  } else*/ {
    //stripLength is in the valid range
    ledstrip.addNewStrip(indexStrip,stripLength);
    Ack();
  }
}


//Clears the data for all configured ledstrip
void  ClearAllLedData() {
  /*for (word ledNr = 0; ledNr < configuredStripLength * NUMBER_LEDSTRIP; ledNr++) {
    ledstrip.setPixel(ledNr, 0);
  }*/
  ledstrip.clearAll();
  Ack();
}


//Sends the firmware version
void SendVersion() {
  Serial.write(FirmwareVersionMajor);
  Serial.write(FirmwareVersionMinor);
  Ack();
}

//Sends the max number of ledstrip per strip
void SendMaxNumberOfLeds() {
  byte B = MaxLedsPerStrip >> 8;
  Serial.write(B);
  B = MaxLedsPerStrip & 255;
  Serial.write(B);
  Ack();
}


//Sends a ack (A)
void Ack() {
  Serial.write('A');
}

//Sends a NACK (N)
void Nack() {
  Serial.write('N');
}

//Receives 3 bytes of color data.
int ReceiveColorData() {
  while (!Serial.available()) {};
  int colorValue = Serial.read();
  while (!Serial.available()) {};
  colorValue = (colorValue << 8) | Serial.read();
  while (!Serial.available()) {};
  colorValue = (colorValue << 8) | Serial.read();
  return colorValue;
}

//Receives a word value. High byte first, low byte second
word ReceiveWord() {
  while (!Serial.available()) {};
  word wordValue = Serial.read() << 8;
  while (!Serial.available()) {};
  wordValue = wordValue | Serial.read();
  return wordValue;
}
