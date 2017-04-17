#define BLYNK_PRINT Serial
#define PIN 6
#include <BlynkSimpleSerialBLE.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <IRremote.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

SoftwareSerial mp3Serial(12, 13);
SoftwareSerial SerialBLE(7, 8); // RX, TX

Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, PIN, NEO_GRB + NEO_KHZ800);
WidgetLCD lcd(V5);

char auth[] = "e50cf81a9fe947419d23ad9a726f3198";

//IR Send
IRsend irsend;

//IR
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
boolean checkIR = false;

int red;
int green;
int blue;

BLYNK_WRITE(V4)
{
  int i = param.asInt();
  String num = param.asStr();
  Serial.println(i);
  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(4, 0, "Sound play");
  lcd.print(3, 1, "Track Num ");
  lcd.print(13, 1, i);
  if ( i == 1) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("play");
      mp3_play(1);
      delay (4000);
      mp3_stop();
    }
    sendIR(0xa);
  } else if (i == 2) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("play");
      mp3_play(2);
      delay (4000);
      mp3_stop();
    }
    sendIR(0xb);
  } else if (i == 3) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 255));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("play");
      mp3_play(3);
      delay (4000);
      mp3_stop();
    }
    sendIR(0xc);
  } else if (i == 4) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 0));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("play");
      mp3_play(4);
      delay (7000);
      mp3_stop();
    }
    sendIR(0xd);
  } else if (i == 5) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 255));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("play");
      mp3_play(5);
      delay (5000);
      mp3_stop();
    }
    sendIR(0xe);
  } else if (i == 6) {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 255));
      strip.show();
    }
    mp3Serial.listen();
    if (mp3Serial.isListening()) {
      Serial.println("play");
      mp3_play(6);
      delay (5000);
      mp3_stop();
    }
    sendIR(0xf);
  }
  for (int i = 0; i < 5; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
  lcd.clear();
  lcd.print(2, 0, "Select Track");
}


BLYNK_WRITE(V2)
{
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.println("Color is: ");
  red = param[0].asInt();
  green = param[1].asInt();
  blue = param[2].asInt();

  Serial.print("red : ");
  Serial.println(red);
  Serial.print("green : ");
  Serial.println(green);
  Serial.print("blue : ");
  Serial.println(blue);

  for (int i = 0; i < 5; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
    strip.show();
  }
}

void setup()
{
  initialize();
  mp3Init();
}

void loop()
{
  SerialBLE.listen();
  Blynk.run();
}


void sendIR(int hex) {
  for (int i = 0 ; i < 3; i++) {
    irsend.sendSony(hex, 20);
    delay(100);
  }
  delay(500);
  Serial.println("IR has been sent");
  Serial.println(hex, HEX);
  irrecv.enableIRIn();
}

void initialize() {
  Serial.begin(9600);
  SerialBLE.begin(9600);
  Blynk.begin(SerialBLE, auth);
  strip.begin();
  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(6, 0, "Hello");
}

void mp3Init() {
  mp3Serial.begin(9600);
  mp3_set_serial (mp3Serial);
  delay(1);
  mp3_set_volume (15);
}

