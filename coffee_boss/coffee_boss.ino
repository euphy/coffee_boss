// Include Emon Library
#include "EmonLib.h"

// This is for the ADC that the scale attaches to.
#include "HX711.h"
#include <RunningMedian.h> // https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian

// For i2c realtime clock
#include <Wire.h> 
#include "RTClib.h"

// For the i2c proximity sensor
#include "Adafruit_VCNL4010.h"

// SD Card setup.
#include "FS.h"
#include "SPIFFS.h"
#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h> // Hardware-specific library

// NFC peripheral
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

EnergyMonitor emon1;
const int CURRENT_SENSOR_PIN = 14;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 15;
HX711 scale;

// Clock setup. Doesn't mention pins because uses default 21/22 on ESP32.
RTC_DS3231 rtc;

// proximity sensor
Adafruit_VCNL4010 vcnl;

// RFID reader
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);
String uidString = "";
boolean useNfc = false;

RunningMedian medianFilteredWeight(8);
RunningMedian medianFilterInterval(6);

float scaleUnitValue = 22.062;

long sensorsReadInterval = 100L;
long sensorsLastReadTime = 0L;
long regularLogInterval = 2000L;
long regularLogLastTime = 0L;

boolean logScaleReadings = true;
boolean logRegularReadings = true;
boolean logChangeReadings = true;

// SD card variables
const int sdChipSelectPin = 25;
File regularDataFile;
File changeDataFile;
File usersDataFile;
File usernamesDataFile;

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
char todayFilenameUsers[18];
char filenameUsernames[18] = "usernames.csv";
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
    Serial.println("Found RFID reader board");
    useNfc = true;
    // Got ok data, print it out!
    Serial.print("Found chip PN5"); 
    Serial.println((versiondata>>24) & 0xFF, HEX); 
    Serial.print("Firmware ver. "); 
    Serial.print((versiondata>>16) & 0xFF, DEC); 
    Serial.print('.'); 
    Serial.println((versiondata>>8) & 0xFF, DEC);
    
    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    nfc.setPassiveActivationRetries(0x4);
    
    // configure board to read RFID tags
    nfc.SAMConfig();
  }
  else {
    Serial.println("___ Didn't find PN53x board ___");
    useNfc = false;
  }
  

//  testMeasurementSpeed();
//  testMeasurementSpeed();
//  testMeasurementSpeed();
}

void loop() {

  rfid_checkForCard();
  
  if (millis() > sensorsLastReadTime + sensorsReadInterval) {
    sensors_measureWeight();
    sensors_logWeight();
    sensors_senseHeaterState();
    sensorsLastReadTime = millis();
  }
  
  lcd_updateDisplay();  
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
