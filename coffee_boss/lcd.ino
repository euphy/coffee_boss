void lcd_init() {
  lcd.init();
  lcd.setRotation(3);
  lcd.setTextDatum(TC_DATUM);  
}

void lcd_displayWeight(float weight) {
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_WHITE);
  lcd.drawNumber(weight, 160, 220, 2);
}
