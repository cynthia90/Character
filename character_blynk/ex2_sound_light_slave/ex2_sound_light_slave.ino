#define PIN 6
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <IRremote.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

SoftwareSerial mp3Serial(12, 13);
SoftwareSerial btSerial(7, 8);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, PIN, NEO_GRB + NEO_KHZ800);

int red;
int green;
int blue;

//IR
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
boolean checkIR = false;

void setup() {
  Serial.begin(9600);
  mp3Serial.begin(9600);
  mp3_set_serial (mp3Serial);
  delay(1);
  mp3_set_volume (5);
  irrecv.enableIRIn();
  strip.begin();
}

void loop() {
  if (receiveIR(0xa) == true) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("Play");
      mp3_play(1);
      delay (4000);
      mp3_stop();
    }
  } else if (receiveIR(0xb) == true) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("Play");
      mp3_play(2);
      delay (4000);
      mp3_stop();
    }
  } else if (receiveIR(0xc) == true) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 255));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("Play");
      mp3_play(3);
      delay (4000);
      mp3_stop();
    }
  } else if (receiveIR(0xd) == true) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 0));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("Play");
      mp3_play(4);
      delay (4000);
      mp3_stop();
    }
  } else if (receiveIR(0xe) == true) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 255));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("Play");
      mp3_play(5);
      delay (4000);
      mp3_stop();
    }
  } else if (receiveIR(0xf) == true) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 255));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("Play");
      mp3_play(6);
      delay (4000);
      mp3_stop();
    }
  }
  for (int i = 0; i < 5; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
}

boolean receiveIR(int hex) {
  checkIR = false;
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if (results.value == hex) {
      Serial.println("Hello");
      checkIR = true;
    }
    irrecv.resume();
  }
  delay(30);
  return checkIR;
}
