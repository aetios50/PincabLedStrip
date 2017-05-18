/********************************************************************************************************
** Arduino Ledstrip Controller
********************************************************************************************************/
#define SERIAL_BUFFER_SIZE 1024

#include <elapsedMillis.h>

#include "LedStrip.h" //A slightly hacked version of the OctoWS2811 lib which allows for dynamic setting of the number of ledstrip is used.

//Definiton of Major and Minor part of the firmware version. This value can be received using the V command.
//If something is changed in the code the number should be increased.
#define FirmwareVersionMajor 1
#define FirmwareVersionMinor 1

//Defines the max number of ledstrip which is allowed per ledstriptrip.
//This number is fine for Teensy 3.2, 3.1. For newer Teensy versions (they dont exists yet) it might be possible to increase this number.
#define MaxLedsPerStrip 1100
//TODO FIXME

//Defines the Pinnumber to which the built in led of the Teensy is connected.
//For Teensy 3.2, 3.1 this is pin 13, if you use a newer Teensy version (not available at the time of writing) you might need to change this number.
#define LedPin 0 // 13

//Variable used to control the blinking and flickering of the led of the Teensy
elapsedMillis BlinkTimer;
int BlinkMode;
elapsedMillis BlinkModeTimeoutTimer;

#define UPDATES_PER_SECOND 100

CRGB leds[MaxLedsPerStrip];
LedStrip ledstrip(MaxLedsPerStrip,leds);

word configuredStripLength=53; // Nombre de leds par strip

//Setup of the system. Is called once on startup.
void setup() {
        Serial.begin(9600);
        while(Serial.available()) {Serial.read();};
        delay(100);
        Serial.println("");
/**/
//#ifdef DEBUG_ON_WIFI
//        wifidebug.begin();
//#endif
/**/

        //Initialize the lib for the ledstrip
        ledstrip.setStripLength(configuredStripLength);
        ledstrip.begin();
        ledstrip.show();

        //Initialize the led pin
        pinMode(LedPin,OUTPUT);

        SetBlinkMode(0);

       // #ifdef DEBUG_ON_WIFI
       // wifidebug.tcp_debug("Setup done");
       // #endif
        /****/
        ClearAllLedData();
        ledstrip.show();
        for(int i=0;i<10;i++){
          ledstrip.setPixel(i, 255, 0, 0);
        }
        ledstrip.show();
        delay(1000);
        ClearAllLedData();
        ledstrip.show();
        for(int i=0;i<10;i++){
          ledstrip.setPixel(i, 0, 255, 0);
        }
        ledstrip.show();
        delay(1000);
        ClearAllLedData();
        ledstrip.show();
        for(int i=0;i<10;i++){
          ledstrip.setPixel(i, 0, 0, 255);
        }
        ledstrip.show();
        delay(1000);
        ClearAllLedData();
        /**/
}

static byte receivedByte;

//Main loop of the programm gets called again and again.
void loop() {
       // ArduinoOTA.handle();
        //Check if data is available
        if (Serial.available() > 0) {

                receivedByte = Serial.read();
//#ifdef DEBUG_ON_WIFI
//                wifidebug.tcp_receive_debug(receivedByte);
//#endif
                switch (receivedByte) {
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
        BlinkMode=Mode;
        BlinkModeTimeoutTimer=0;
}

//Controls the blinking of the led
void Blink() {
        switch(BlinkMode) {
        case 0:
                //Blinkmode 0 is only active after the start of the Teensy until the first command is received.
                if(BlinkTimer<1500) {
                        digitalWrite(LedPin,0);
                } else if(BlinkTimer<1600) {
                        digitalWrite(LedPin,1);
                } else {
                        BlinkTimer=0;
                        digitalWrite(LedPin,0);
                }
                break;
        case 1:
                //Blinkmode 1 is activated when the Teensy receives a command
                //Mode expires 500ms after the last command has been received resp. mode has been set
                if(BlinkTimer>30) {
                        BlinkTimer=0;
                        digitalWrite(LedPin,!digitalRead(LedPin));
                }
                if(BlinkModeTimeoutTimer>500) {
                        SetBlinkMode(2);
                }
                break;
        case 2:
                //Blinkmode 2 is active while the Teensy is waiting for more commands
                if(BlinkTimer<1500) {
                        digitalWrite(LedPin,0);
                } else if(BlinkTimer<1600) {
                        digitalWrite(LedPin,1);
                } else if(BlinkTimer<1700) {
                        digitalWrite(LedPin,0);
                } else if(BlinkTimer<1800) {
                        digitalWrite(LedPin,1);
                }else {
                        BlinkTimer=0;
                        digitalWrite(LedPin,0);
                }
        default:
                //This should never be active
                //The code is only here to make it easier to determine if a wrong Blinkcode has been set
                if(BlinkTimer>2000) {
                        BlinkTimer=0;
                        digitalWrite(LedPin,!digitalRead(LedPin));
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
        word firstLed=ReceiveWord();

        word numberOfLeds=ReceiveWord();

        int ColorData=ReceiveColorData();

        if( firstLed<=configuredStripLength*8 && numberOfLeds>0 && firstLed+numberOfLeds-1<=configuredStripLength*8 ) {
                word endLedNr=firstLed+numberOfLeds;
                for(word ledNr=firstLed; ledNr<endLedNr; ledNr++) {
                        ledstrip.setPixel(ledNr,ColorData);
                }
                Ack();
        } else {
                //Number of the first led or the number of ledstrip to receive is outside the allowed range
                Nack();
        }


}


//Receives data for the ledstriptrips
void ReceiveData() {
        word firstLed=ReceiveWord();

        word numberOfLeds=ReceiveWord();
/*
 #ifdef DEBUG_ON_WIFI
                     wifidebug.tcp_receive_word_debug(firstLed);
                     wifidebug.tcp_receive_word_debug(numberOfLeds);
 #endif
 */
        if( firstLed<=configuredStripLength*8 && numberOfLeds>0 && firstLed+numberOfLeds-1<=configuredStripLength*8 ) {
                //FirstLedNr and numberOfLeds are valid.
                //Receive and set color data

                word endLedNr=firstLed+numberOfLeds;
/*
 #ifdef DEBUG_ON_WIFI
                                     wifidebug.tcp_receive_word_debug(endLedNr);
 #endif
 */
                int i=0;
                for(word ledNr=firstLed; ledNr<endLedNr; ledNr++) {
                  i++;
/*
 #ifdef DEBUG_ON_WIFI
                  wifidebug.tcp_receive_word_debug(ledNr);
 #endif
 */
                        ledstrip.setPixel(ledNr,ReceiveColorData());
                }

                #ifdef DEBUG_ON_WIFI
                wifidebug.tcp_receive_color_debug(i);
                #endif

                Ack();

        } else {
                //Number of the first led or the number of ledstrip to receive is outside the allowed range
                Nack();
        }
}

//Sets the length of the longest connected ledstriptrip. Length is restricted to the max number of allowed ledstrip
void SetLedStripLength() {
        word stripLength=ReceiveWord();
        if(stripLength<1 || stripLength>MaxLedsPerStrip) {
                //stripLength is either to small or above the max number of ledstrip allowed
                Nack();
        } else {
                //stripLength is in the valid range
                configuredStripLength=stripLength;
                ledstrip.setStripLength(stripLength);
                ledstrip.begin(); //Reinitialize the OctoWS2811 lib (not sure if this is needed)

                Ack();
        }
}

//Clears the data for all configured ledstrip
void  ClearAllLedData() {
        for(word ledNr=0; ledNr<configuredStripLength*8; ledNr++) {
                ledstrip.setPixel(ledNr,0);
        }
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
        byte B=MaxLedsPerStrip>>8;
        Serial.write(B);
        B=MaxLedsPerStrip&255;
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
        while(!Serial.available()) {};
        int colorValue=Serial.read();
        while(!Serial.available()) {};
        colorValue=(colorValue<<8)|Serial.read();
        while(!Serial.available()) {};
        colorValue=(colorValue<<8)|Serial.read();
        return colorValue;
}

//Receives a word value. High byte first, low byte second
word ReceiveWord() {
        while(!Serial.available()) {};
        word wordValue=Serial.read()<<8;
        while(!Serial.available()) {};
        wordValue=wordValue|Serial.read();
        return wordValue;
}
