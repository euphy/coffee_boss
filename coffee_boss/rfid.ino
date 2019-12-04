void rfid_checkForCard() {
  
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");

    for (uint8_t i = 0; i < uidLength; i++) {
      if(uid[i] < 0x10) 
        uidString.concat("0");
      
      uidString += String(uid[i], HEX);
    }
    Serial.println(uidString);
    
    // log bump to file
    rfid_logCardBumpToFile(uidString);

    // look up user in users file
    String username = sd_getUsernameForUid(uidString);
    Serial.print("UID ");
    Serial.print(uidString);
    Serial.print(" has a friendly name of ");
    Serial.print(username);
    Serial.println(".");

    // show a summary of the users activity on the LCD for a few seconds
    lcd_showUserSummary(uidString, username, 6000);

  }
  else
  {
    // PN532 probably timed out waiting for a card
    //Serial.println("Timed out waiting for a card");
  }
  uidString = "";
}

void rfid_logCardBumpToFile(String uid) {
    usersDataFile = SD.open(todayFilenameUsers, FILE_APPEND);
    if (usersDataFile) {
      Serial.print("Writing to ");
      Serial.println(todayFilenameUsers);
      
      usersDataFile.print(dateString);
      usersDataFile.print('T');
      usersDataFile.print(timeString);
      usersDataFile.print(',');

      usersDataFile.print(dateString);
      usersDataFile.print(',');
      
      usersDataFile.print(timeString);
      usersDataFile.print(',');
  
      usersDataFile.println(uid);
      usersDataFile.close();
    }
    else {
      usersDataFile = SD.open(todayFilenameUsers, FILE_WRITE);
      if (usersDataFile) {
        Serial.print("Creating ");
        Serial.println(todayFilenameUsers);
        usersDataFile.println("date, time, weight");
        usersDataFile.close();
      }
      else {
        Serial.print("Problem creating file: ");
        Serial.println(todayFilenameUsers);
      }
    }  
}
