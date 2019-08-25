
void rtc_serialPrintTime(DateTime t) {
  Serial.print(t.year(), DEC);
  Serial.print('/');
  Serial.print(t.month(), DEC);
  Serial.print('/');
  Serial.print(t.day(), DEC);
  Serial.print(' ');
  Serial.print(t.hour(), DEC);
  Serial.print(':');
  Serial.print(t.minute(), DEC);
  Serial.print(':');
  Serial.print(t.second(), DEC);
  Serial.println();
}
