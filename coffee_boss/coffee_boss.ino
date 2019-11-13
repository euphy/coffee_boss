#include "EmonLib.h"                   // Include Emon Library

// This is for the ADC that the scale attaches to.
#include "HX711.h"
#include <RunningMedian.h> // https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian

// For i2s realtime clock
#include <Wire.h> 
#include "RTClib.h"

// Clock setup. Doesn't mention pins because uses default 21/22 on ESP32.
RTC_DS3231 rtc;

// SD Card setup.
#include "FS.h"
#include "SPIFFS.h"
#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h> // Hardware-specific library


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 15;
HX711 scale;

EnergyMonitor emon1;


RunningMedian medianFilteredWeight(8);
RunningMedian medianFilterInterval(6);

float scaleUnitValue = 22.062;

long scaleReadInterval = 100L;
long scaleLastReadTime = 0L;
long regularLogInterval = 2000L;
long regularLogLastTime = 0L;

boolean logScaleReadings = true;
boolean logRegularReadings = true;
boolean logChangeReadings = true;

// SD card variables
const int sdChipSelectPin = 25;
File regularDataFile;
File changeDataFile;
boolean cardPresent = false;
boolean cardInit = false;


// LCD defined
TFT_eSPI lcd = TFT_eSPI();       // Invoke custom library

// This is the file name used to store the touch coordinate
// calibration data. Cahnge the name to start a new calibration.
#define CALIBRATION_FILE "/PolargraphCalData" // TouchCalData3

// Set recalibrateTouchScreen to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
boolean recalibrateTouchScreen = false;


float changeThreshold = 30; // change must be more than this to trigger an event
float lastMeasuredWeight = 0.0;
float lastSettledWeight = 0.0;
int lastFilteredWeight = 0;
int lastDisplayedWeight = 0;
int filteredWeight = 0;
int measurementInterval = 0;

DateTime currentTime;
char todayFilenameRegular[18];
char todayFilenameChange[18];
char dateString[11];
char timeString[9];

void setup() {
  Serial.begin(57600);
  Serial.println("\n\n\nCoffee Boss is going to BOSS your COFFEE.");

  lcd_init();
  // This is the file name used to store the touch coordinate
  // calibration data. Cahnge the name to start a new calibration.
  #define CALIBRATION_FILE "/PolargraphCalData" // TouchCalData3  


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

  emon1.current(14, 111.1);

  sd_simpleInit();

//  testMeasurementSpeed();
//  testMeasurementSpeed();
//  testMeasurementSpeed();
}


void loop() {
  
  if (millis() > scaleLastReadTime + scaleReadInterval) {
    currentTime = rtc.now();
    lastMeasuredWeight = scale.get_units(1);
    
    lastFilteredWeight = filteredWeight;
    medianFilteredWeight.add((int)lastMeasuredWeight);
    filteredWeight = medianFilteredWeight.getMedian();
    
    medianFilterInterval.add(millis() - scaleLastReadTime);
    measurementInterval = medianFilterInterval.getMedian();

    double irms = emon1.calcIrms(1480)*230;
    rtc_serialPrintTime(currentTime, false);
    Serial.print(millis());
    Serial.print("\t| measured:\t");
    Serial.print(lastMeasuredWeight, 1);
    Serial.print("g\t| filtered:\t");
    Serial.print(filteredWeight);
    Serial.print("g\t| power:\t");
    Serial.println(irms);
    sd_prepareFilenames();

    sd_logRegularValue();
    sd_logChangeValue();

    lcd_updateDisplay();    
    scaleLastReadTime = millis();
    if ((int)filteredWeight != (int)lastFilteredWeight) {
      lastMeasuredWeight = filteredWeight;
    }
  }
}


void testMeasurementSpeed() {
  /*
   * Power up and down reliably takes 0ms.
   * Reading getUnits takes different times
   * 1   - 398ms
   * 5   - 757ms (151ms each)
   * 10  - 1204ms (120ms each)
   * 50  - 4788ms (95ms each)
   * 100 - 9273ms (93ms each)
   */
  long before = millis();
  scale.power_up();
  long afterPowerUp = millis();
  scale.get_units(100);
  long afterGetUnits = millis();
  scale.power_down();
  long afterPowerDown = millis();

  Serial.print("Power up: ");
  Serial.print(afterPowerUp - before);
  Serial.print(", measure: ");
  Serial.print(afterGetUnits - afterPowerUp);
  Serial.print(", power down: ");
  Serial.println(afterPowerDown - afterGetUnits);
  Serial.print("Took ");
  Serial.print(afterPowerDown - before);
  Serial.println("ms in total");

}
