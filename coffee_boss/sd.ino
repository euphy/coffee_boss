
void sd_simpleInit() {

  cardPresent = false;
  cardInit = false;
  pinMode(sdChipSelectPin, OUTPUT); // necessary for SD card reading to work

  // see if the card is present and can be initialized:
  int initValue = 0;
  initValue = SD.begin(sdChipSelectPin);
  if (initValue == 0) {
    Serial.println("Card failed, or not present");
  }
  else {
    Serial.println("Successfully beginned.");
    cardPresent = true;
  }

  if (cardPresent) {
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.println("card initialized.");
    File root = SD.open("/", FILE_READ);

    cardInit = true;

    sd_printDirectory(root, 0);
    root.close();
    Serial.println("done!");
  }
}


void sd_printDirectory(File dir, int numTabs)
{
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       Serial.println("...");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print("File found: ");
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       sd_printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void sd_logData(char *dateStr, char *timeStr, float weight, float current, float light, int proximity)
{
  return;
}

void sd_logRegularValue() {

  if (millis() > regularLogLastTime + regularLogInterval) {
  
    regularDataFile = SD.open(todayFilenameRegular, FILE_APPEND);

    if (regularDataFile) {
      Serial.print("Writing to ");
      Serial.println(todayFilenameRegular);
      
      regularDataFile.print(dateString);
      regularDataFile.print('T');
      regularDataFile.print(timeString);
      regularDataFile.print(',');
      
      regularDataFile.print(dateString);
      regularDataFile.print(',');
      
      regularDataFile.print(timeString);
      regularDataFile.print(',');

      regularDataFile.print(lastMeasuredWeight);
      regularDataFile.print(',');
      regularDataFile.print(lastMeasuredCurrent);
      regularDataFile.print(',');
      regularDataFile.print(lastMeasuredAmbientLight);
      regularDataFile.print(',');
      regularDataFile.println(lastMeasuredProximity);

      regularDataFile.close();
    }
    else {
      regularDataFile = SD.open(todayFilenameRegular, FILE_WRITE);
      if (regularDataFile) {
        Serial.print("Creating ");
        Serial.println(todayFilenameRegular);
        regularDataFile.println("date, time, weight");
        regularDataFile.close();
      }
      else {
        Serial.print("Problem creating file: ");
        Serial.println(todayFilenameRegular);
      }
    }
    regularLogLastTime = millis();
  }  
}

void sd_logChangeValue() {

  if (abs(lastFilteredWeight - lastSettledWeight) > changeThreshold) {
    Serial.print("Weight has changed from ");
    Serial.print(lastSettledWeight);
    Serial.print(" to ");
    Serial.print(lastFilteredWeight);
    Serial.print(" so I'll log it in ");
    Serial.println(todayFilenameChange);
    // log the change
    lastSettledWeight = lastFilteredWeight;
    changeDataFile = SD.open(todayFilenameChange, FILE_APPEND);
    if (changeDataFile) {
      Serial.print("Writing to ");
      Serial.println(todayFilenameChange);
      
      changeDataFile.print(dateString);
      changeDataFile.print('T');
      changeDataFile.print(timeString);
      changeDataFile.print(',');

      changeDataFile.print(dateString);
      changeDataFile.print(',');
      
      changeDataFile.print(timeString);
      changeDataFile.print(',');
  
      changeDataFile.println(lastMeasuredWeight);
      changeDataFile.close();
    }
    else {
      changeDataFile = SD.open(todayFilenameChange, FILE_WRITE);
      if (changeDataFile) {
        Serial.print("Creating ");
        Serial.println(todayFilenameChange);
        changeDataFile.println("date, time, weight");
        changeDataFile.close();
      }
      else {
        Serial.print("Problem creating file: ");
        Serial.println(todayFilenameChange);
      }
    }  
  }
}

void sd_prepareTimeString(DateTime currentTime, char *timeStringName)
{
  sprintf(timeStringName, "%02u:%02u:%02u", currentTime.hour(), currentTime.minute(), currentTime.second());
}
void sd_prepareDateString(DateTime currentTime, char *dateStringName)
{
  sprintf(dateStringName, "%04u-%02u-%02u", currentTime.year(), currentTime.month(), currentTime.day());
}

void sd_prepareFilenames(DateTime currentTime, char *filenameRegularName, char * filenameChangeName) {
  
  sprintf(filenameRegularName, "/datr%04u%02u%02u.csv", currentTime.year(), currentTime.month(), currentTime.day());
  sprintf(filenameChangeName, "/datc%04u%02u%02u.csv", currentTime.year(), currentTime.month(), currentTime.day());

//  Serial.print("todayFilenameRegular: ");
//  Serial.println(todayFilenameRegular);
//  Serial.print("todayFilenameChange: ");
//  Serial.println(todayFilenameChange);
//  Serial.print("dateString: ");
//  Serial.println(dateString);
//  Serial.print("timeString: ");
//  Serial.println(timeString);
//  Serial.print(".");

}
