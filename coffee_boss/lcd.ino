void lcd_init() {
  lcd.init();
  lcd.setRotation(3);
  lcd.setTextDatum(BC_DATUM);  
}

void lcd_displayWeight(float weight) {
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_WHITE);
  lcd.setTextSize(6);
  lcd.drawNumber(weight, 160, 220, 2);
}
