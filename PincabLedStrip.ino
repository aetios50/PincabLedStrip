/********************************************************************************************************
** Pincab Ledstrip Controller
********************************************************************************************************/
/*
 * This code is fully compatible with WEMOS D1 PRO
 */

#define SERIAL_BUFFER_SIZE 2048

#include <EEPROM.h>
#include <elapsedMillis.h>

/***/
//#define DEBUG_ON_WIFI
#ifdef DEBUG_ON_WIFI
#include "WifiDebug.h"
static WifiDebug wifidebug;
#endif
/***/

#include "LedStrip.h"

#define FirmwareVersionMajor 2
#define FirmwareVersionMinor 0

//Defines the Pinnumber to which the built in led
#define LedPin LED_BUILTIN

#define READ_EEPROM_SETTINGS 0

enum SettingsEnum{
  TEST_ON_RESET,
  TEST_SWITCH,
  ACTIVITY_LED,
  SETTING4,
  SETTING5,
  SETTING6,
  SETTING7,
  SETTING8,
  COUNT
};

uint8_t Settings[SettingsEnum::COUNT] = {
  1,
  0,
  1,
  0,
  0,
  0,
  0,
  0
};

void ReadSettings(){
#if READ_EEPROM_SETTINGS
  EEPROM.begin(SettingsEnum::COUNT);
  for(int i = 0; i < SettingsEnum::COUNT; ++i){
    Settings[i] = EEPROM.read(i);
  }
  EEPROM.end();
#endif

  Serial.print("Test on Reset :");
  Serial.println(Settings[SettingsEnum::TEST_ON_RESET], DEC);
  Serial.print("Test Switch :");
  Serial.println(Settings[SettingsEnum::TEST_SWITCH], DEC);
  Serial.print("Activity Led :");
  Serial.println(Settings[SettingsEnum::ACTIVITY_LED], DEC);
}

bool HasSetting(int setting){
  return Settings[setting] != 0;
}

// Defines the Pinnumber for the test button which is low when pressed
#define TestPin D0

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

  ReadSettings();

  /**/
#ifdef DEBUG_ON_WIFI
  wifidebug.begin();
#endif
  /**/

  //Initialize the lib for the ledstrip
  ledstrip.setStripLength(configuredStripLength);
  ledstrip.begin();
  ledstrip.show();

  if (HasSetting(SettingsEnum::ACTIVITY_LED)){
    //Initialize the led pin
    pinMode(LedPin, OUTPUT);
  }

  SetBlinkMode(0);

#ifdef DEBUG_ON_WIFI
  wifidebug.debug_send_msg("Setup done");
#endif

  if (HasSetting(SettingsEnum::TEST_SWITCH)){
    //Initialize and find value of the test pin
    pinMode(TestPin,INPUT_PULLUP); 
    digitalWrite(TestPin, HIGH); 
  }

  if (HasSetting(SettingsEnum::TEST_ON_RESET)){
    Test();
  }

  ClearAllLedData();
  ledstrip.show();
}

void TestLedstripColor(byte r, byte g, byte b){
  ledstrip.clearAll();
  ledstrip.show();
  Blink();
  FastLED.delay(200);
  for(int i = 0; i < configuredStripLength * NUMBER_LEDSTRIP; i++){
    ledstrip.setPixel(i,r, g, b);
  }
  ledstrip.show();
  Blink();
  FastLED.delay(200);
  ledstrip.clearAll();
  ledstrip.show();
}

void Test() {
  TestLedstripColor(BRIGHTNESS, 0, 0);
  TestLedstripColor(0, BRIGHTNESS, 0);
  TestLedstripColor(0, 0, BRIGHTNESS);
}

static byte receivedByte;

//Main loop of the programm gets called again and again.
void loop() {

  // run test if button is grounded
  if (HasSetting(SettingsEnum::TEST_SWITCH) && digitalRead(TestPin)==LOW) { 
      Test();
  }

  //Check if data is available
  if (Serial.available() > 0) {

    receivedByte = Serial.read();

    switch (receivedByte) {
      case 'Z':
        //Set length of a strip
        SetALedStripLength();
        break;
      case 'B':
        //Set brightness of a strip
        SetALedStripBrightness();
        break;
      case 'L':
        //Set length of strips
        SetLedStripLength();
        break;
      case 'F':
        //Fill strip area with color
        Fill();
        break;
      case 'Q':
        //receive compressed data for strips
        ReceiveCompressedData();
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
      case 'T':
        //Launch a Test sequence
        Test();
        Ack();
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

void ActivityLed(int activity)
{
  if (!HasSetting(SettingsEnum::ACTIVITY_LED)) return;

  if (activity < 0){
    digitalWrite(LedPin, !digitalRead(LedPin));
  } else{
    digitalWrite(LedPin, activity);
  }
}

//Controls the blinking of the led
void Blink() {
  switch (BlinkMode) {
    case 0:
      //Blinkmode 0 is only active after the start of the Wemos until the first command is received.
      if (BlinkTimer < 1500) {
        ActivityLed(0);
      } else if (BlinkTimer < 1600) {
        ActivityLed(1);
      } else {
        BlinkTimer = 0;
        ActivityLed(0);
      }
      break;
    case 1:
      //Blinkmode 1 is activated when the Wemos receives a command
      //Mode expires 500ms after the last command has been received resp. mode has been set
      if (BlinkTimer > 30) {
        BlinkTimer = 0;
        ActivityLed(-1);
      }
      if (BlinkModeTimeoutTimer > 500) {
        SetBlinkMode(2);
      }
      break;
    case 2:
      //Blinkmode 2 is active while the Wemos is waiting for more commands
      if (BlinkTimer < 1500) {
        ActivityLed(0);
      } else if (BlinkTimer < 1600) {
        ActivityLed(1);
      } else if (BlinkTimer < 1700) {
        ActivityLed(0);
      } else if (BlinkTimer < 1800) {
        ActivityLed(1);
      } else {
        BlinkTimer = 0;
        ActivityLed(0);
      }
    default:
      //This should never be active
      //The code is only here to make it easier to determine if a wrong Blinkcode has been set
      if (BlinkTimer > 2000) {
        BlinkTimer = 0;
        ActivityLed(-1);
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
void ReceiveCompressedData() {
  word firstLed = ReceiveWord();
  word numberOfCompressedData = ReceiveWord();
  word numberOfLeds = ReceiveWord();
  if ( firstLed <= configuredStripLength * NUMBER_LEDSTRIP && numberOfLeds > 0 && firstLed + numberOfLeds - 1 <= configuredStripLength * NUMBER_LEDSTRIP ) {
    word startLed = firstLed;
    word endLedNr = firstLed + numberOfLeds;
    for (word numPack = 0; numPack < numberOfCompressedData; numPack++){
       while (!Serial.available()) {};
       uint8_t nbLeds = (uint8_t)Serial.read();
       int color = ReceiveColorData();
       
       for (word numLed = 0; numLed < nbLeds; numLed++){
         ledstrip.setPixel(startLed + numLed, color);
       }
       startLed += nbLeds;
       
       if (startLed >= endLedNr){
        break;        
       }
    }
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

//Sets the brightness of a led strip
void SetALedStripBrightness() {
  while (!Serial.available()) {};
  byte indexStrip = Serial.read();  while (!Serial.available()) {};
  byte lastStrip = Serial.read();
  while (!Serial.available()) {};
  uint8_t brightness = Serial.read();
  ledstrip.setStripBrightness(indexStrip,brightness);
  Ack();
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

  //Reset all ledstrips to 0 size until ledstrip size are sent per index
  ledstrip.reset();

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
