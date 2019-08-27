// This is for the ADC that the scale attaches to.
#include "HX711.h"

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

float scaleUnitValue = 22.062;

long scaleReadInterval = 2000L;
long scaleLastReadTime = 0L;

boolean logScaleReadings = true;

// SD card variables
const int sdChipSelectPin = 25;
File dataFile;
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


void setup() {
  Serial.begin(57600);
  Serial.println("\nCoffee Boss is going to BOSS your COFFEE.");

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

  sd_simpleInit();
  
}

float currentWeight = 0.0;
DateTime currentTime;
char todayFilename[18];
char dateString[11];
char timeString[9];

void loop() {
  if (millis() > scaleLastReadTime + scaleReadInterval) {
    scale.power_up();
    currentTime = rtc.now();
    currentWeight = scale.get_units(10);
    lcd_displayWeight(currentWeight);
    rtc_serialPrintTime(currentTime, false);
    Serial.print("\t| average:\t");
    Serial.print(currentWeight, 1);
    Serial.println("grams");

    sprintf(todayFilename, "/data%04u%02u%02u.csv", currentTime.year(), currentTime.month(), currentTime.day());
    sprintf(dateString, "%04u-%02u-%02u", currentTime.year(), currentTime.month(), currentTime.day());
    sprintf(timeString, "%02u:%02u:%02u", currentTime.hour(), currentTime.minute(), currentTime.second());

    Serial.print("todayFilename: ");
    Serial.println(todayFilename);
    Serial.print("dateString: ");
    Serial.println(dateString);
    Serial.print("timeString: ");
    Serial.println(timeString);
    
    dataFile = SD.open(todayFilename, FILE_APPEND);

    if (dataFile) {
      Serial.print("Writing to ");
      Serial.println(todayFilename);
      
      dataFile.print(dateString);
      dataFile.print(',');
      
      dataFile.print(timeString);
      dataFile.print(',');

      dataFile.println(currentWeight);
      dataFile.close();
    }
    else {
      Serial.print("Couldn't find file (");
      Serial.print(todayFilename);
      Serial.println("), creating it.");
      dataFile = SD.open(todayFilename, FILE_WRITE);
      if (dataFile) {
        Serial.print("Creating ");
        Serial.println(todayFilename);
        dataFile.println("timestamp, weight");
        dataFile.close();
      }
      else {
        Serial.print("Problem creating file: ");
        Serial.println(todayFilename);
      }
      
    }
        
    scale.power_down();
    scaleLastReadTime = millis();
  }
}
