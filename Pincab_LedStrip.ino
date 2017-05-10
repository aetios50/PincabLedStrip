#include <FastLED.h>
// Liste des constantes à compléter en fonction de votre config
// ------------------------------------------------------------
// définie l'ordre des couleurs pris en charge par le LEDSTRIP
#define COLOR_ORDER GRB
// définie le type de chipset du LEDSTRIP
#define CHIPSET     WS2812B
// définie les Pins pour les datas des LEDSTRIP
// dans ma config
#define LED_PIN_1     0 // ledstrip playfield gauche
#define LED_PIN_2     2 // ledstrip playfield droit
#define LED_PIN_3     3 // ledstrip fronton
#define LED_PIN_4     4 // ledring gauche
#define LED_PIN_5     5 // ledring droit
// définie le nombre de leds par ledstrip (GD = gauche et droit | F = Fronton) 
#define NUM_LEDS_PER_STRIP_GD 53
// #define NUM_LEDS_PER_STRIP_F 14
// #define NUM_LEDS_PER_RING 20
// définie le nombre total de leds
#define NUM_LEDS 106
// définie le nombre de LED par ZONE, soit 1/5e de chaque ledstrip
#define LED_BY_ZONE NUM_LEDS_PER_STRIP_GD / 5
// définie le numéro de la première LED de la ZONE (une ZONE = 1/5e de chaque côté du playfield)
// la configuration de ce code considère que les ledstrips 1 et 2 sont assignés de chaque côté du playfield
// respectivement ledstrip 1 = gauche et ledstrip 2 = droit
#define LED_ZONE0 0
#define LED_ZONE1 NUM_LEDS_PER_STRIP_GD / 5
#define LED_ZONE2 NUM_LEDS_PER_STRIP_GD * 2 / 5
#define LED_ZONE3 NUM_LEDS_PER_STRIP_GD * 3 / 5
#define LED_ZONE4 NUM_LEDS_PER_STRIP_GD * 4 / 5
#define LED_ZONE5 NUM_LEDS_PER_STRIP_GD
#define LED_ZONE6 NUM_LEDS_PER_STRIP_GD * 6 / 5
#define LED_ZONE7 NUM_LEDS_PER_STRIP_GD * 7 / 5
#define LED_ZONE8 NUM_LEDS_PER_STRIP_GD * 8 / 5
#define LED_ZONE9 NUM_LEDS_PER_STRIP_GD * 9 / 5

CRGB leds[NUM_LEDS];

void setup() {
  // tell FastLED there's the LED number on LEDSTRIP n°1 leds on pin 0, starting at index 0 in the led array
  FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(leds, 0, NUM_LEDS_PER_STRIP_GD);
  // tell FastLED there's 60 NEOPIXEL leds on pin 11, starting at index 60 in the led array
  FastLED.addLeds<CHIPSET, LED_PIN_2, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP_GD, NUM_LEDS_PER_STRIP_GD);
  // tell FastLED there's 60 NEOPIXEL leds on pin 11, starting at index 60 in the led array
  // FastLED.addLeds<CHIPSET, LED_PIN_3, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP_GD * 2, NUM_LEDS_PER_STRIP_F);
  // définie les pins suivant comme entrées d'impulsion depuis la LEDWIZ
  // pinMode(22, INPUT_PULLUP); // Gyro
  pinMode(24, INPUT_PULLUP); // Strobe
  pinMode(26, INPUT_PULLUP); // Shaker
  pinMode(28, INPUT_PULLUP); // Slingshot gauche
  pinMode(30, INPUT_PULLUP); // Slingshot droit
  pinMode(32, INPUT_PULLUP); // Bumper milieu gauche
  pinMode(34, INPUT_PULLUP); // Bumper milieu centre
  pinMode(36, INPUT_PULLUP); // Bumper milieu droit
  pinMode(38, INPUT_PULLUP); // Bumper arrière gauche
  pinMode(40, INPUT_PULLUP); // Bumper arrière centre
  pinMode(42, INPUT_PULLUP); // Bumper arrière droit
  pinMode(44, INPUT_PULLUP); // Bell
//  pinMode(46, INPUT_PULLUP);
//  pinMode(48, INPUT_PULLUP);
//  pinMode(50, INPUT_PULLUP);
//  pinMode(52, INPUT_PULLUP);
}

void loop() 
{
  if (digitalRead(23) == LOW) {Chenillard();}
  if (digitalRead(24) == HIGH) {Strobe();}
  if (digitalRead(26) == HIGH) {Strobe();}
  if (digitalRead(28) == HIGH) {SlingshotLeft();}
  if (digitalRead(30) == HIGH) {SlingshotRight();}
  if (digitalRead(32) == HIGH) {Bumper(LED_ZONE2);}
  if (digitalRead(34) == HIGH) {Bumper(LED_ZONE3);}
  if (digitalRead(36) == HIGH) {Bumper(LED_ZONE7);}
  if (digitalRead(38) == HIGH) {Bumper(LED_ZONE4);}
  if (digitalRead(40) == HIGH) {Bumper(LED_ZONE8);}
  if (digitalRead(42) == HIGH) {Bumper(LED_ZONE9);}
  if (digitalRead(44) == HIGH) {Strobe();}
  delay(100);
     
  for (int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB::Black;
    }
  FastLED.show();
  delay(50);
}

void Chenillard() {
// Cette fonction génère un effet type chenille sur le ledstrip droit. 
// Cet effet est prévu accompagner le lancement de la bille, après appuie du Bouton Launch;
  for(int i = LED_ZONE5; i < LED_ZONE5 + NUM_LEDS_PER_STRIP_GD - 2; i++) {
    leds[i + 2] = CRGB::Red;
    leds[i + 1] = CRGB::Orange;
    leds[i] = CRGB::Yellow;
    FastLED.show();
    delay(10);
    leds[i] = CRGB::Black;
    leds[i + 1] = CRGB::Black;
    leds[i + 2] = CRGB::Black;
    FastLED.show();
  }
}

void Strobe() {
// Cette fonction génère un effet stroboscopique de l'ensemble des LEDS;

  for(int i = 0; i < 5; i++) {
    for (int il = 0; il < 53; il++){
      leds[il] = CRGB::White;
    }
    FastLED.show();
    delay(50);
    
    for (int il = 0; il < 120; il++){
      leds[il] = CRGB::Black;
    }
    FastLED.show();
    delay(50);
  }
}

void Bumper(int LED_ZONE) {
// Cette fonction génère un effet popur le bumper milieu droit;

  int X = random(6);
  for (int i = LED_ZONE; i < LED_ZONE + LED_BY_ZONE; i++){
      if (X == 0) {leds[i] = CRGB::White;}
      if (X == 1) {leds[i] = CRGB::Red;}
      if (X == 2) {leds[i] = CRGB::Green;}
      if (X == 3) {leds[i] = CRGB::Blue;}
      if (X == 4) {leds[i] = CRGB::Yellow;}  
      if (X == 5) {leds[i] = CRGB::Orange;}
      if (X == 6) {leds[i] = CRGB::Purple;}
  }
  FastLED.show();
}

void SlingshotRight() {
// Cette fonction génère un effet attribuée au slingshot droit;
  uint8_t colorIndex;
  int n = LED_ZONE6;
  int nmax = LED_ZONE6 + LED_BY_ZONE;
  int nmiddle = LED_ZONE6 + LED_BY_ZONE / 2;
  for(int i = 0; i < LED_BY_ZONE / 2; i++) 
    {
      leds[nmiddle + i] = ColorFromPalette(RainbowColors_p, colorIndex, 200, LINEARBLEND);
      leds[nmiddle - i] = ColorFromPalette(RainbowColors_p, colorIndex, 200, LINEARBLEND);
      colorIndex += 20;
      FastLED.show();
      delay(50);
     }
 }

void SlingshotLeft() {
// Cette fonction génère un effet attribuée au slingshot droit;
  uint8_t colorIndex;
  int n = LED_ZONE1;
  int nmax = LED_ZONE1 + LED_BY_ZONE;
  int nmiddle = LED_ZONE1 + LED_BY_ZONE / 2;
  for(int i = 0; i < LED_BY_ZONE / 2; i++) 
    {
      leds[nmiddle + i] = ColorFromPalette(RainbowColors_p, colorIndex, 200, LINEARBLEND);
      leds[nmiddle - i] = ColorFromPalette(RainbowColors_p, colorIndex, 200, LINEARBLEND);
      colorIndex += 20;
      FastLED.show();
      delay(50);
    }
}

void Gyro() {
// Cette fonction génère un effet attribuée au gyrophare;
   for(int n = 0; n < 3; n++) 
    {
      for(int i = 0; i < 7; i++) 
      {
        leds[i] = CRGB::Red;  
      }
      FastLED.show();
      delay(20);
      
      for(int i = 0; i < 7; i++) 
      {
        leds[i] = CRGB::Black;  
      }
      FastLED.show();
      delay(100);
    }
    
for(int n = 0; n < 3; n++) 
    {
     for(int i = 7; i < 14; i++) 
      {
        leds[i] = CRGB::Blue;
      }
        FastLED.show();
        delay(20);
      
     for(int i = 7; i < 14; i++) 
      {
        leds[i] = CRGB::Black;
      }
        FastLED.show();
        delay(100);
    }
}
