void lcd_init() {
  lcd.init();
  lcd.setRotation(3);
  lcd.setTextDatum(BC_DATUM);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.setTextWrap(false);
}

void lcd_displayWeight(float weight) {
  lcd.setTextSize(6);
  lcd.setCursor(20, 100);
  lcd.print(weight, 0);
  lcd.print("g      ");
}

void lcd_displayMeasurementInterval() {
  lcd.setTextSize(2);
  lcd.setCursor(10, 10);
  lcd.print("Measurement rate ");
  lcd.print(1000.0/(float)measurementInterval, 2);
  lcd.print("hz");
}

void lcd_updateDisplay() {
  if ((int)lastDisplayedWeight != (int)lastFilteredWeight) {
    Serial.print("lastFilteredWeight: ");
    Serial.println(lastFilteredWeight);
    lcd.fillScreen(TFT_BLACK);
    lcd_displayWeight(lastFilteredWeight);
    lastDisplayedWeight = lastFilteredWeight;
    lcd_displayMeasurementInterval();
  }
  
}
