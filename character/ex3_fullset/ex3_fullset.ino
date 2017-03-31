#define BLYNK_PRINT Serial
#define PIN 6
#include <BlynkSimpleSerialBLE.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define lightPin A1

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "e50cf81a9fe947419d23ad9a726f3198";

SoftwareSerial SerialBLE(7, 8); // RX, TX
SoftwareSerial mp3Serial(12, 13);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, PIN, NEO_GRB + NEO_KHZ800);
WidgetLCD lcd(V5);
tmElements_t tm;

int red;
int green;
int blue;

//Time Check
unsigned long onTime = millis(); //Light on time
unsigned long OffTime = 0; //Light off time
unsigned long delayTime = 60000; //15min

boolean lightState = false;//light state check
boolean beforeLightState = false;
boolean isLightOn = false;

int rtcDOW = 0;
boolean blynkDOW[] = {false, false, false, false, false, false, false, false};
int blynkhour;
int blynkmin;

boolean connectioncheck = false;
boolean sensorMode = false;
boolean alarmCheck = false;

int lightVal = 0;

BLYNK_WRITE(V1) {
  TimeInputParam t(param);

  if (t.hasStartTime())
  {
    //요일 초기화
    for (int i = 0 ; i < 7; i++) {
      blynkDOW[i] = false;
    }
    Serial.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
    blynkhour = t.getStartHour();
    blynkmin = t.getStartMinute();
  }
  else if (t.isStartSunrise())
  {
    Serial.println("Start at sunrise");
  }
  else if (t.isStartSunset())
  {
    Serial.println("Start at sunset");
  }
  else
  {
    // Do nothing
  }

  // Process stop time

  if (t.hasStopTime())
  {
    Serial.println(String("Stop: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());
  }
  else if (t.isStopSunrise())
  {
    Serial.println("Stop at sunrise");
  }
  else if (t.isStopSunset())
  {
    Serial.println("Stop at sunset");
  }
  else
  {
    // Do nothing: no stop time was set
  }

  // Process timezone
  // Timezone is already added to start/stop time

  Serial.println(String("Time zone: ") + t.getTZ());

  // Get timezone offset (in seconds)
  Serial.println(String("Time zone offset: ") + t.getTZ_Offset());

  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)

  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      Serial.println(String("Day ") + i + " is selected");
      blynkDOW[i] = true;
    }
  }
  //알람설정 되면 lcd에 표시
  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(4, 0, "Alarm Set");
  lcd.print(6, 1, blynkhour);
  lcd.print(8, 1, ":");
  lcd.print(9, 1, blynkmin);
  Serial.println();
}


BLYNK_WRITE(V2)
{
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

  for (int i = 0; i < 5; i++) { //빛켜기
    strip.setPixelColor(i, strip.Color(red, green, blue));
    strip.show();
  }

  //무드등 켜진 시간 체크
  isLightOn = true;
  onTime = millis();

  //색이 바뀌면 lcd에 표시
  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(3, 0, "Mood Light");
  /*
    lcd.print(0, 1, "r:");
    lcd.print(2, 1, red);
    lcd.print(6, 1, "g:");
    lcd.print(8, 1, green);
    lcd.print(11, 1, "b:");
    lcd.print(13, 1, blue);
  */
}

//스위치로 모드 변경(센서모드, 무드등모드)
BLYNK_WRITE(V3)
{
  String i = param.asStr();
  if (i == "1") {
    Serial.println("Sensor Mode");
    lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(3, 0, "Sensor Mode");
    delay(3000);
    sensorMode = true;
    isLightOn = true;
  } else {
    Serial.println("Mood Light Mode");
    lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(1, 0, "Mood Light Mode");
    delay(3000);
    sensorMode = false;
  }
}

BLYNK_WRITE(V4)
{
  // You can also use:
  Serial.print("Music Play : ");
  String i = param.asStr();
  Serial.println(i);
}


//빛센서값 블링크 앱으로 읽기
BLYNK_READ(V0)
{
  Blynk.virtualWrite(V0, lightVal);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  SerialBLE.begin(9600);
  Blynk.begin(SerialBLE, auth);
  strip.begin();
  mp3Serial.begin(9600);
  mp3_set_serial (mp3Serial);
  delay(1);
  mp3_set_volume (10);
  Serial.println("Waiting for connections...");
  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(2, 0, "Waiting for");
  lcd.print(4, 1, "connections...");
  connectioncheck = true;
  for (int i = 0; i < 5; i++) { //빛끄기
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
}

void loop()
{
  SerialBLE.listen();
  Blynk.run();
  if (connectioncheck == true) {
    lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(5, 0, "Hello");
    delay(3000);
    connectioncheck = false;
  }

  //1. 빛센서 감지 모드일 때
  if (sensorMode == true) {
    //빛센서값 감지하기
    lightVal = analogRead(lightPin);
    delay(100);

    if (lightVal > 100) {
      for (int i = 0; i < 5; i++) { //밝을 때 빛 끄기
        strip.setPixelColor(i, strip.Color(0, 0, 0));
        strip.show();
      }
      isLightOn = true;
      lightState = false;
    } else {
      lightState = true;
    }

    //자동끄기 기능
    //이전 상태와 다를 경우에만 불을 켠다.
    //주변이 밝았다가 어두워진 경우
    //주변이 밝은 경우에는 무조건 불을 끈다.
    if (lightState != beforeLightState) {
      if (lightState == true) {
        onTime = millis();
        Serial.println("Time Start");
        for (int i = 0; i < 5; i++) { //빛켜기
          strip.setPixelColor(i, strip.Color(255, 255, 255));
          strip.show();
        }
        mp3Serial.listen();
        if (mp3Serial.isListening()) {
          Serial.println("Play");
          mp3_play(1);
          delay (4000);
          mp3_stop();
        }
      }
    }

    if (millis() - onTime > delayTime) {
      isLightOn = false;
      Serial.println("Time Off (Sensor Mode)");
    }

    if (isLightOn == false) {
      for (int i = 0; i < 5; i++) { //빛끄기
        strip.setPixelColor(i, strip.Color(0, 0, 0));
        strip.show();
      }
    }
    beforeLightState = lightState;
  }

  //2. 무드등 모드 자동 끄기 기능
  if (millis() - onTime > delayTime) {
    isLightOn = false;
    Serial.println("Time Off (Mood Light Mode)");
  }
  if (isLightOn == false) {
    for (int i = 0; i < 5; i++) { //빛끄기
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.show();
    }
  }

  //3. 알람 기능
  if (RTC.read(tm)) {
    rtcDOW = DayofDate(tmYearToCalendar(tm.Year), tm.Month, tm.Day);
    //시간, 요일 체크하기
    for (int i = 1; i <= 7; i++) {
      if (blynkDOW[i] == true) {
        if (i == rtcDOW) {
          if ((blynkhour == tm.Hour) && (blynkmin == tm.Minute)) {
            isLightOn = true; //빛센서 모드의 OFF와 겹치지 않게 하기 위해서
            alarmCheck = true;
          }
        }
      }
    }
  }

  if (alarmCheck == true) {
    red = 0;
    green = 0;
    blue = 0;
    Serial.print("RTC :  ");
    Serial.print(rtcDOW);
    Serial.print("  ");
    Serial.print(tm.Hour);
    Serial.print("  ");
    Serial.println(tm.Minute);
    for (int i = 0; i < 5; i++) { //빛켜기
      red += 50;
      green += 50;
      blue += 50;
      strip.setPixelColor(i, strip.Color(red, green, blue));
      strip.show();
      Serial.println("Alarm!!");
      delay(20000);
      mp3Serial.listen();
      if (mp3Serial.isListening()) {
        Serial.println("Play");
        mp3_play(1);
        delay (4000);
        mp3_stop();
      }
    }
    for (int i = 0; i < 5; i++) { //빛끄기
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.show();
    }
    alarmCheck = false;
  }
  delay(1000);
}


void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

int DayofDate(int y, int m, int d) {
  int DOW = 0;
  long totalDay = 0;
  for (int i = 1900; i <= y; i++) {
    //입력 년도 이전 까지는 12월 까지 다 더함
    if (i < y) {
      for (int j = 1; j <= 12; j++) {
        totalDay += MonthDays(i, j);
      }
      //입력 년도는 입력 월 이전까지 더함
    } else {
      for (int j = 1; j < m; j++) {
        totalDay += MonthDays(i, j);
      }
    }
  }
  //나머지 일 수를 구해진 전체 일수에 더함
  totalDay += d;
  //기준일인 1900년 1월 1일이 월요일이므로
  DOW = totalDay % 7;
  return DOW;
}

int MonthDays(int y, int m) {
  int dayOfMonth = 0;
  //윤년일 조건
  boolean ly = ((y % 4 == 0) && (y % 100 != 0)) || y % 400 == 0;
  //31일 일 조건
  boolean is31 = (m == 1) || (m == 3) || (m == 5) || (m == 7)
                 || (m == 8) || (m == 10) || (m == 12);

  if (ly && m == 2) {
    dayOfMonth = 29;
  } else if (!ly && m == 2) {
    dayOfMonth = 28;
  } else if (is31) {
    dayOfMonth = 31;
  } else {
    dayOfMonth = 30;
  }
  return dayOfMonth;
}


