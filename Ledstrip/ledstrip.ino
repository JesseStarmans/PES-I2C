#include <FastLED.h>

#define LED_PIN     D4
#define NUM_LEDS    60
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define VERGROTING 20
#define OMHOOG D2
#define OMLAAG D3
#define AANUIT D1

int helderheid = 128; 
int aan = 0;

void setup() {
  delay(3000); 
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(helderheid);
  fill_solid(leds, NUM_LEDS, CRGB::Blue);  

  pinMode(OMHOOG, INPUT_PULLUP);
  pinMode(OMLAAG, INPUT_PULLUP);
  pinMode(AANUIT, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(OMHOOG) == LOW) {
    helderheid += VERGROTING;
  if (helderheid > 255) {
    helderheid = 255;
  }
    FastLED.setBrightness(helderheid);
    FastLED.show();
    delay(100); 
  }

  if (digitalRead(AANUIT) == LOW) {
    if(helderheid > 0) {
      helderheid = 0;
    }
    else{
      helderheid = 128;
    }
    FastLED.setBrightness(helderheid);
    FastLED.show();
    delay(1000); 
  }

  if (digitalRead(OMLAAG) == LOW) {
    helderheid -= VERGROTING;
  if (helderheid < 0) {
    helderheid = 0;
  }
    FastLED.setBrightness(helderheid);
    FastLED.show();
    delay(100); 
  }
  delay(10);
}
