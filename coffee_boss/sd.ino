
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
