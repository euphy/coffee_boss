// This is for the ADC that the scale attaches to.
#include "HX711.h"

// For i2s realtime clock
#include <Wire.h> 
#include "RTClib.h"

// Clock setup. Doesn't mention pins because uses default 21/22 on ESP32.
RTC_DS3231 rtc;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 15;
HX711 scale;

float scaleUnitValue = 22.062;

long scaleReadInterval = 2000L;
long scaleLastReadTime = 0L;

boolean logScaleReadings = true;


void setup() {
  Serial.begin(57600);
  Serial.println("Coffee Boss is going to BOSS your COFFEE.");


  // Setup scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(scaleUnitValue);
  scale.tare();


  // setup RTC 
  Wire.begin();
  boolean rtcBegun = rtc.begin();

  if (rtcBegun) {
    Serial.println("RTC is started");
  } else {
    Serial.println("RTC couldn't be found!");
    while (1); 
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the default time.");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  } else {
    Serial.println("Picked up time from RTC: ");
    rtc_serialPrintTime(rtc.now());
  }
}



void loop() {
  if (millis() > scaleLastReadTime + scaleReadInterval) {
    scale.power_up();
    Serial.print("one reading:\t");
    Serial.print(scale.get_units(), 1);
    Serial.print("grams\t| average:\t");
    Serial.print(scale.get_units(10), 1);
    Serial.println("grams");
    scale.get_units(10);
    scale.power_down();
    scaleLastReadTime = millis();
  }
}
