#include "EmonLib.h"                   // Include Emon Library

// This is for the ADC that the scale attaches to.
#include "HX711.h"
#include <RunningMedian.h> // https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian

// For i2c realtime clock
#include <Wire.h> 
#include "RTClib.h"

// For the i2c proximity sensor
#include "Adafruit_VCNL4010.h"

// Clock setup. Doesn't mention pins because uses default 21/22 on ESP32.
RTC_DS3231 rtc;

// SD Card setup.
#include "FS.h"
#include "SPIFFS.h"
#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 15;
HX711 scale;

EnergyMonitor emon1;
const int CURRENT_SENSOR_PIN = 14;

Adafruit_VCNL4010 vcnl;

String uidString = "";
boolean useNfc = true;

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

const int HEATER_CURRENT_THRESHOLD = 40000;
float lastMeasuredCurrent = 0.0;
float lastDisplayedCurrent = 0.0;
RunningMedian medianCurrent(8);
float filteredCurrent = 0.0;

boolean useProximitySensor = false;
const int PROXIMITY_THRESHOLD = 6000;
int lastMeasuredProximity = 0;
int lastMeasuredAmbientLight = 0;
int lastDisplayedProximity = 0;
int lastDisplayedAmbientLight = 0;

DateTime heaterStartedTime = 0L;
DateTime heaterStoppedTime = 0L;
char heaterTimeString[9];
boolean heaterRunning = false;



void setup() {
  Serial.begin(57600);
  Serial.println("\n\n\nCoffee Boss is going to BOSS your COFFEE.");

  lcd_init();

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

  emon1.current(CURRENT_SENSOR_PIN, 111.1);

  if (! vcnl.begin()){
    Serial.println("Sensor not found :(");
    useProximitySensor = false;
    while (1);
  }
  else {
    useProximitySensor = true;
    Serial.println("Found VCNL4010");
    vcnl.setLEDcurrent(20);
  }
  
  
  sd_simpleInit();

  Serial.println("Setting up RFID reader");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (versiondata) {
    useNfc = true;
    // Got ok data, print it out!
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
    Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
    Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
    
    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    nfc.setPassiveActivationRetries(0xFF);
    
    // configure board to read RFID tags
    nfc.SAMConfig();    
  } else {
    Serial.print("Didn't find PN53x board");
    useNfc = false;
  }  

//  testMeasurementSpeed();
//  testMeasurementSpeed();
//  testMeasurementSpeed();
}


void loop() {

  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX); 
    }
    Serial.println("");
    // Wait 1 second before continuing
    delay(1000);
  }
  else
  {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
  }
  
  if (millis() > scaleLastReadTime + scaleReadInterval) {
    currentTime = rtc.now();
    lastMeasuredWeight = scale.get_units(1);
    
    lastFilteredWeight = filteredWeight;
    medianFilteredWeight.add((int)lastMeasuredWeight);
    filteredWeight = medianFilteredWeight.getMedian();
    
    medianFilterInterval.add(millis() - scaleLastReadTime);
    measurementInterval = medianFilterInterval.getMedian();

    lastMeasuredCurrent = emon1.calcIrms(1480)*230;
    medianCurrent.add(lastMeasuredCurrent);
    filteredCurrent = medianCurrent.getMedian();

    if (useProximitySensor) {
      lastMeasuredAmbientLight = vcnl.readAmbient();
      lastMeasuredProximity = vcnl.readProximity();
    }

    rtc_serialPrintTime(currentTime, false);

    Serial.print(millis());
    Serial.print("\t| measured:\t");
    Serial.print(lastMeasuredWeight, 1);
    Serial.print("g\t| filtered:\t");
    Serial.print(filteredWeight);
    Serial.print("g\t| power:\t");
    Serial.print(lastMeasuredCurrent);
    Serial.print("\t| ambient:\t ");
    Serial.print(lastMeasuredAmbientLight);
    Serial.print("\t| Proximity:\t");
    Serial.println(lastMeasuredProximity);
    
    sd_prepareFilenames(currentTime, todayFilenameRegular, todayFilenameChange);
    sd_prepareTimeString(currentTime, timeString);
    sd_prepareDateString(currentTime, dateString);

    sd_logData(dateString, timeString, lastMeasuredWeight, lastMeasuredCurrent, lastMeasuredAmbientLight, lastMeasuredProximity);

    senseHeaterState();

    sd_logRegularValue();
    sd_logChangeValue();

    scaleLastReadTime = millis();
    if ((int)filteredWeight != (int)lastFilteredWeight) {
      lastMeasuredWeight = filteredWeight;
    }
  }
  
  lcd_updateDisplay();  
}

void senseHeaterState() {
  if ((filteredCurrent > HEATER_CURRENT_THRESHOLD) && !heaterRunning) {
    heaterStartedTime = rtc.now();
    sd_prepareTimeString(heaterStartedTime, heaterTimeString);
    heaterRunning = true;
  }
  else if ((filteredCurrent < HEATER_CURRENT_THRESHOLD) && heaterRunning) {
    heaterStoppedTime = rtc.now();
    sd_prepareTimeString(heaterStoppedTime, heaterTimeString);
    heaterRunning = false;
    scale.tare();
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
