// Wifi Lamp Project

// Vagtsal, 14/7/2017

// Arduino Mini Code
// Receives commands from ESP8266 through serial connection and drives 6 strips of 18 addressable leds (WS2812B chipset)
// It supports multiple colors, effects and brightness control
// Current state is stored in EEPROM to revert after a power loss

#include <FastLED.h>
#include <EEPROM.h>

#define BRIGHT_ADDR    200
#define RED_ADDR       201
#define GREEN_ADDR     202
#define BLUE_ADDR      203
#define EFFECT_ADDR    204
#define WEATHER_ADDR   205

#define NO_EFFECT    0
#define FIRE         1
#define WEATHER      2

#define CLEAR         1
#define CLOUDS        2
#define RAIN          3
#define THUNDERSTORM  4
#define SNOW          5

#define LED_PIN1     3
#define LED_PIN2     4
#define LED_PIN3     5
#define LED_PIN4     6
#define LED_PIN5     7
#define LED_PIN6     8
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    18

#define COOLING  55
#define SPARKING 120
#define FRAMES_PER_SECOND 120

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];
CRGB leds5[NUM_LEDS];
CRGB leds6[NUM_LEDS];

CRGB color;
int effect;
int brightness;
int weather;

String inputString = "";

void change_state(String choice){
   if (choice == "r\n"){
     color.r = 255;
     EEPROM.write(RED_ADDR, 255);
   }
   else if (choice == "nr\n"){
     color.r = 0;
     EEPROM.write(RED_ADDR, 0);
   }
   else if (choice == "g\n"){
     color.g = 255;
     EEPROM.write(GREEN_ADDR, 255);
   }
   else if (choice == "ng\n"){
     color.g = 0;
     EEPROM.write(GREEN_ADDR, 0);
   }
   else if (choice == "b\n"){
     color.b = 255;
     EEPROM.write(BLUE_ADDR, 255);
   }
   else if (choice == "nb\n"){
     color.b = 0;
     EEPROM.write(BLUE_ADDR, 0);
   }
   else if (choice == "bu\n"){
     if (brightness <= 75){brightness += 25;}else{brightness = 100;}
     EEPROM.write(BRIGHT_ADDR, brightness);
   }
   else if (choice == "bd\n"){
     if (brightness >= 25){brightness -= 25;}else{brightness = 0;}
     EEPROM.write(BRIGHT_ADDR, brightness);
   }
   else if (choice == "f\n"){
     effect = FIRE;
     EEPROM.write(EFFECT_ADDR, FIRE);
   }
   else if (choice == "nf\n"){
     effect = NO_EFFECT;
     EEPROM.write(EFFECT_ADDR, NO_EFFECT);
   }
   else if (choice == "w\n"){
     effect = WEATHER;
     EEPROM.write(EFFECT_ADDR, WEATHER);
   }
   else if (choice == "nw\n"){
     effect = NO_EFFECT;
     EEPROM.write(EFFECT_ADDR, NO_EFFECT);
   }
   
   else if (choice == "ws\n"){
     weather = CLEAR;
     EEPROM.write(WEATHER_ADDR, weather);
   }
   else if (choice == "wc\n"){
     weather = CLOUDS;
     EEPROM.write(WEATHER_ADDR, weather);
   }
   else if (choice == "wr\n"){
     weather = RAIN;
     EEPROM.write(WEATHER_ADDR, weather);
   }
   else if (choice == "wt\n"){
     weather = THUNDERSTORM;
     EEPROM.write(WEATHER_ADDR, weather);
   }
   else if (choice == "wx\n"){
     weather = SNOW;
     EEPROM.write(WEATHER_ADDR, weather);
   }
   
   if (effect == NO_EFFECT){
     for( int j = 0; j < NUM_LEDS; j++) {
       leds1[j] = color;
       leds2[j] = color;
       leds3[j] = color;
       leds4[j] = color;
       leds5[j] = color;
       leds6[j] = color;
      }
   }
   
   // exponential mapping
   int expBrightness = brightness * brightness;
   FastLED.setBrightness(map(expBrightness, 0, 10000, 0, 143));
   
   FastLED.show();
}


void Fire2012(CRGB leds[NUM_LEDS]){
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      leds[j] = color;
    }
}

void weather_effect(CRGB leds[NUM_LEDS], int period0, int period1, int period2, int period3, int period4, int period5){
  for( int i = 0; i < NUM_LEDS; i++) {
    switch (i){
      case 0:
      case 5:
      case 2:
        if (weather == CLEAR || weather == CLOUDS){
          leds[i].g = period0;
        }
        if (weather == CLOUDS){
          leds[i].r = period0;
        }
        break;
      case 1:
      case 17:
      case 15:
        if (weather == CLEAR || weather == CLOUDS){
          leds[i].g = period1;
        }
        if (weather == CLOUDS){
          leds[i].r = period1;
        }
        break;
      case 12:
      case 7:
      case 10:
        if (weather == CLEAR || weather == CLOUDS){
          leds[i].g = period2;
        }
        if (weather == CLOUDS){
          leds[i].r = period2;
        }
        break;
      case 3:
      case 9:
      case 13:
        if (weather == CLEAR || weather == CLOUDS){
          leds[i].g = period3;
        }
        if (weather == CLOUDS){
          leds[i].r = period3;
        }
        break;
      case 4:
      case 14:
      case 8:
        if (weather == CLEAR || weather == CLOUDS){
          leds[i].g = period4;
        }
        if (weather == CLOUDS){
          leds[i].r = period4;
        }
        break;
      case 6:
      case 16:
      case 11:
        if (weather == CLEAR || weather == CLOUDS){
          leds[i].g = period5;
        }
        if (weather == CLOUDS){
          leds[i].r = period5;
        }
        break;
    }
    if (weather == CLEAR){
      leds[i].b = 0;
      leds[i].r = 255;
    }
    else if (weather == CLOUDS){
       leds[i].b = 200;
    }
  }
}


void raining(){
  for (int i=0; i<NUM_LEDS; i++){
    if (leds1[i].b == 255){
      leds1[i].b = 15;
      if (i != 0){
        leds1[i-1].b = 255;
      }
    }
    else {
      leds1[i].r = 0;
      leds1[i].g = 0;
      leds1[i].b = 15;
    }
    if (leds2[i].b == 255){
      leds2[i].b = 15;
     if (i != 0){
        leds2[i-1].b = 255;
      }
    }
    else {
      leds2[i].r = 0;
      leds2[i].g = 0;
      leds2[i].b = 15;
    }
    if (leds3[i].b == 255){
      leds3[i].b = 15;
      if (i != 0){
        leds3[i-1].b = 255;
      }
    }
    else {
      leds3[i].r = 0;
      leds3[i].g = 0;
      leds3[i].b = 15;
    }
    if (leds4[i].b == 255){
      leds4[i].b = 15;
      if (i != 0){
        leds4[i-1].b = 255;
      }
    }
    else {
      leds4[i].r = 0;
      leds4[i].g = 0;
      leds4[i].b = 15;
    }
    if (leds5[i].b == 255){
      leds5[i].b = 15;
      if (i != 0){
        leds5[i-1].b = 255;
      }
    }
    else {
      leds5[i].r = 0;
      leds5[i].g = 0;
      leds5[i].b = 15;
    }
    if (leds6[i].b == 255){
      leds6[i].b = 15;
      if (i != 0){
        leds6[i-1].b = 255;
      }
    }
    else {
      leds6[i].r = 0;
      leds6[i].g = 0;
      leds6[i].b = 15;
    }
  }   
  int randomizeTime = random8(100);
  if (randomizeTime > 90){
    int randomize = random8(6);
    switch (randomize){
      case 0:
        leds1[NUM_LEDS-1].b = 255;
        break;
      case 1:
        leds2[NUM_LEDS-1].b = 255;
        break;
      case 2:
        leds3[NUM_LEDS-1].b = 255;
        break;
      case 3:
        leds4[NUM_LEDS-1].b = 255;
        break;
      case 4:
        leds5[NUM_LEDS-1].b = 255;
        break;
      case 5:
        leds6[NUM_LEDS-1].b = 255;
        break;
    }
  }
}

void thunder(){
  int randomizeThunder = random8(6);
  for( int i = 0; i < NUM_LEDS; i++) {
    switch (randomizeThunder){
      case 0:
        leds1[i] = CRGB::White;
        break;
      case 1:
        leds2[i] = CRGB::White;
        break;
      case 2:
        leds3[i] = CRGB::White;
        break;
      case 3:
        leds4[i] = CRGB::White;
        break;
      case 4:
        leds5[i] = CRGB::White;
        break;
      case 5:
        leds6[i] = CRGB::White;
        break;
    }
  }
  FastLED.delay(50);
  for( int i = 0; i < NUM_LEDS; i++) {
    switch (randomizeThunder){
      case 0:
        leds1[i] = CRGB::Black;
        leds1[i].b = 15;
        break;
      case 1:
        leds2[i] = CRGB::Black;
        leds2[i].b = 15;
        break;
      case 2:
        leds3[i] = CRGB::Black;
        leds3[i].b = 15;
        break;
      case 3:
        leds4[i] = CRGB::Black;
        leds4[i].b = 15;
        break;
      case 4:
        leds5[i] = CRGB::Black;
        leds5[i].b = 15;
        break;
      case 5:
        leds6[i] = CRGB::Black;
        leds6[i].b = 15;
        break;
    }
  }
}

void snowing(){
  for (int i=0; i<NUM_LEDS; i++){
    if (leds1[i] == CRGB(255,255,255)){
      leds1[i] = CRGB(15,15,15);
      if (i != 0){
        leds1[i-1] = CRGB(255,255,255);
      }
    }
    else {
      leds1[i] = CRGB(15,15,15);
    }
    if (leds2[i] == CRGB(255,255,255)){
      leds2[i] = CRGB(15,15,15);
      if (i != 0){
        leds2[i-1] = CRGB(255,255,255);
      }
    }
    else {
      leds2[i] = CRGB(15,15,15);
    }
    if (leds3[i] == CRGB(255,255,255)){
      leds3[i] = CRGB(15,15,15);
      if (i != 0){
        leds3[i-1] = CRGB(255,255,255);
      }
    }
    else {
      leds3[i] = CRGB(15,15,15);
    }
    if (leds4[i] == CRGB(255,255,255)){
      leds4[i] = CRGB(15,15,15);
      if (i != 0){
        leds4[i-1] = CRGB(255,255,255);
      }
    }
    else {
      leds4[i] = CRGB(15,15,15);
    }
    if (leds5[i] == CRGB(255,255,255)){
      leds5[i] = CRGB(15,15,15);
      if (i != 0){
        leds5[i-1] = CRGB(255,255,255);
      }
    }
    else {
      leds5[i] = CRGB(15,15,15);
    }
    if (leds6[i] == CRGB(255,255,255)){
      leds6[i] = CRGB(15,15,15);
      if (i != 0){
        leds6[i-1] = CRGB(255,255,255);
      }
    }
    else {
      leds6[i] = CRGB(15,15,15);
    }
  }   
  int randomizeTime = random8(100);
  if (randomizeTime > 70){
    int randomize = random8(6);
    switch (randomize){
      case 0:
        leds1[NUM_LEDS-1] = CRGB(255,255,255);
        break;
      case 1:
        leds2[NUM_LEDS-1] = CRGB(255,255,255);
        break;
      case 2:
        leds3[NUM_LEDS-1] = CRGB(255,255,255);
        break;
      case 3:
        leds4[NUM_LEDS-1] = CRGB(255,255,255);
        break;
      case 4:
        leds5[NUM_LEDS-1] = CRGB(255,255,255);
        break;
      case 5:
        leds6[NUM_LEDS-1] = CRGB(255,255,255);
        break;
    }
  }
  FastLED.delay(500);
}

void setup() {
  delay(1000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN1, COLOR_ORDER>(leds1, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN3, COLOR_ORDER>(leds3, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN4, COLOR_ORDER>(leds4, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN5, COLOR_ORDER>(leds5, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN6, COLOR_ORDER>(leds6, NUM_LEDS).setCorrection( TypicalLEDStrip );
  
  /* when eeprom is new, set default values
  EEPROM.write(BRIGHT_ADDR, 50);
  EEPROM.write(RED_ADDR, 1);
  EEPROM.write(GREEN_ADDR, 1);
  EEPROM.write(BLUE_ADDR, 1);
  EEPROM.write(EFFECT_ADDR, NO_EFFECT);
  EEPROM.write(WEATHER_ADDR, CLEAR);
  */
  
  brightness = EEPROM.read(BRIGHT_ADDR);
  color.r = EEPROM.read(RED_ADDR);
  color.g = EEPROM.read(GREEN_ADDR);
  color.b = EEPROM.read(BLUE_ADDR);
  effect = EEPROM.read(EFFECT_ADDR);
  weather = EEPROM.read(WEATHER_ADDR);
  
  Serial.begin(115200);
  inputString.reserve(200);
}

void loop() {
  if (effect == FIRE){
    Fire2012(leds1); 
    Fire2012(leds2); 
    Fire2012(leds3); 
    Fire2012(leds4); 
    Fire2012(leds5); 
    Fire2012(leds6); 
  }
  if (effect == WEATHER){
    int period0;
    int period1;
    int period2;
    int period3;
    int period4;
    int period5;
    
    if (weather == CLEAR || weather == CLOUDS){
      if (weather == CLEAR){
        period0 = beatsin16(3,0,145);
        period1 = beatsin16(11,0,145);
        period2 = beatsin16(17,0,145);
        period3 = beatsin16(23,0,145);
        period4 = beatsin16(31,0,145);
        period5 = beatsin16(49,0,145);
      }
      else if (weather == CLOUDS){
        period0 = beatsin16(3,0,255);
        period1 = beatsin16(11,0,255);
        period2 = beatsin16(17,0,255);
        period3 = beatsin16(23,0,255);
        period4 = beatsin16(31,0,255);
        period5 = beatsin16(49,0,255);
      }
    }
    weather_effect(leds1, period0, period1, period2, period3, period4, period5); 
    weather_effect(leds2, period1, period2, period3, period4, period5, period0); 
    weather_effect(leds3, period2, period3, period4, period5, period0, period1); 
    weather_effect(leds4, period3, period4, period5, period0, period1, period2); 
    weather_effect(leds5, period4, period5, period0, period1, period2, period4); 
    weather_effect(leds6, period5, period0, period1, period2, period3, period4);
  
    if (weather == RAIN || weather == THUNDERSTORM){
      raining();
      if (weather == THUNDERSTORM){
        int randomThunder = random8(100);
        if (randomThunder > 98){
          thunder();
        }        
      }    
      FastLED.delay(1000 / 60);  
    }
    
    else if (weather == SNOW){
      snowing();
    }
    
  }
  
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);  
}


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    inputString += inChar;
    
    if (inChar == '\n') {
      change_state(inputString);
      Serial.print(inputString);
 
      inputString = "";
    } 
  }
}


