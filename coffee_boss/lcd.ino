void lcd_init() {
  lcd.init();
  lcd.setRotation(3);
  lcd_prepareScreenForDataDisplay();
}

void lcd_prepareScreenForDataDisplay() {
  lcd.setTextDatum(BC_DATUM);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextWrap(false);
}

void lcd_displayWeight(float weight) {
  lcd.setTextSize(2);
  lcd.setCursor(10, 50);
  lcd.print("Weight:");
  lcd.setCursor(150, 50);
  lcd.print(weight, 0);
  lcd.print("g      ");
}

void lcd_displayCurrent(float current) {
  lcd.setTextSize(2);
  lcd.setCursor(10, 80);
  lcd.print("Current:");
  lcd.setCursor(150, 80);
  lcd.print(current, 0);
  lcd.print("      ");
}

void lcd_displayLight(int light) {
  lcd.setTextSize(2);
  lcd.setCursor(10, 110);
  lcd.print("Light:");
  lcd.setCursor(150, 110);
  lcd.print(light);
  lcd.print("      ");
}

void lcd_displayProximity(int proximity) {
  lcd.setTextSize(2);
  lcd.setCursor(10, 140);
  lcd.print("Proximity:");
  lcd.setCursor(150, 140);
  lcd.print(proximity);
  lcd.print("      ");
}

void lcd_displayMeasurementInterval() {
  lcd.setTextSize(2);
  lcd.setCursor(10, 10);
  lcd.print("Measurement rate ");
  lcd.print(1000.0/(float)measurementInterval, 2);
  lcd.print("hz     ");
}

void lcd_displayHeaterStatus() {
  lcd.setTextSize(2);
  lcd.setCursor(10, 170);
  
  if (heaterRunning) {
//    lcd.setTextColor(TFT_WHITE, TFT_RED);
    TimeSpan timeSinceHeaterTurnedOn = rtc.now()-heaterStartedTime;
      
    lcd.print("Heater on for ");
    lcd.print(timeSinceHeaterTurnedOn.totalseconds());
    lcd.print("s             ");
  }
  else {
//    lcd.setTextColor(TFT_WHITE, TFT_GREEN);
    TimeSpan timeSinceHeaterTurnedOff = rtc.now()-heaterStoppedTime;
    lcd.print("Coffee made ");
    if (timeSinceHeaterTurnedOff.days() > 0) {
      lcd.print(timeSinceHeaterTurnedOff.days());
      lcd.print(" days ");
    }
    if (timeSinceHeaterTurnedOff.hours() > 0) {
      lcd.print(timeSinceHeaterTurnedOff.hours());
      lcd.print("h ");
    }
    if (timeSinceHeaterTurnedOff.minutes() > 0) {
      lcd.print(timeSinceHeaterTurnedOff.minutes());
      lcd.print("m ");
    }
    if (timeSinceHeaterTurnedOff.seconds() > 0) {
      lcd.print(timeSinceHeaterTurnedOff.seconds());
      lcd.print("s ");
    }
    lcd.print("ago            ");
  }
  
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);    
}

void lcd_updateDisplay() {
  boolean redrawInstantWeight = false;
  
  if ((int)lastDisplayedWeight != (int)lastFilteredWeight) {
    lcd_displayWeight(lastFilteredWeight);
    lastDisplayedWeight = lastFilteredWeight;
    lcd_displayMeasurementInterval();    
  }
  
  if ((int)lastDisplayedCurrent != (int)filteredCurrent) {
    lcd_displayCurrent(filteredCurrent);
    lastDisplayedCurrent = filteredCurrent;
    lcd_displayHeaterStatus();
  }
  
  if ((int)lastDisplayedAmbientLight != (int)lastMeasuredAmbientLight) {
    lcd_displayLight(lastMeasuredAmbientLight);
    lastDisplayedAmbientLight = lastMeasuredAmbientLight;
  }

  if ((int)lastDisplayedProximity != (int)lastMeasuredProximity) {
    lcd_displayProximity(lastMeasuredProximity);
    lastDisplayedProximity = lastMeasuredProximity;
  }  
}

void lcd_showUserSummary(String uidString, String username, int displayDuration) {
  Serial.println("Showing summary");
  lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  lcd.fillScreen(TFT_WHITE);
  lcd.setTextDatum(BC_DATUM);
  lcd.setTextSize(4);
  lcd.setCursor(20, 60);
  lcd.print(uidString);
  lcd.setCursor(20, 120);
  lcd.print(username);
  delay(displayDuration);
  lcd_prepareScreenForDataDisplay();
}
