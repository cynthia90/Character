#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, PIN, NEO_GRB + NEO_KHZ800);

int red = 0;
int green = 0;
int blue = 0;

void setup() {
  strip.begin();
  Serial.begin(9600);
  red = 255;
  green = 255;
  blue = 255;
}

void loop() {
  for (int i = 0; i < 5; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
    strip.show();
  }
}

