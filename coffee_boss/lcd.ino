void lcd_init() {
  lcd.init();
  lcd.setRotation(3);
  lcd.setTextDatum(BC_DATUM);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextWrap(false);
}

void lcd_displayWeight(float weight) {
  lcd.setTextSize(3);
  lcd.setCursor(10, 50);
  lcd.print("Weight:");
  lcd.setCursor(200, 50);
  lcd.print(weight, 0);
  lcd.print("g      ");
}

void lcd_displayCurrent(float current) {
  lcd.setTextSize(3);
  lcd.setCursor(10, 90);
  lcd.print("Current:");
  lcd.setCursor(200, 90);
  lcd.print(current, 0);
  lcd.print("      ");
}

void lcd_displayLight(int light) {
  lcd.setTextSize(3);
  lcd.setCursor(10, 130);
  lcd.print("Light:");
  lcd.setCursor(200, 130);
  lcd.print(light);
  lcd.print("      ");
}

void lcd_displayProximity(int proximity) {
  lcd.setTextSize(3);
  lcd.setCursor(10, 170);
  lcd.print("Proximity:");
  lcd.setCursor(200, 170);
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

void lcd_updateDisplay() {
  boolean redrawInstantWeight = false;
  
  if ((int)lastDisplayedWeight != (int)lastFilteredWeight) {
    lcd_displayWeight(lastFilteredWeight);
    lastDisplayedWeight = lastFilteredWeight;
    lcd_displayMeasurementInterval();    
  }
  
  if ((int)lastDisplayedCurrent != (int)lastMeasuredCurrent) {
    lcd_displayCurrent(lastMeasuredCurrent);
    lastDisplayedCurrent = lastMeasuredCurrent;
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
