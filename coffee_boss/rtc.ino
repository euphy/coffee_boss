void rtc_serialPrintTime(DateTime t) {
  rtc_serialPrintTime(t, true);
}

void rtc_serialPrintTime(DateTime t, boolean endLine) {
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
  if (endLine) {
    Serial.println();
  }
}
//
//#define SDA_LOW()   (GPES = (1 << SDA))
//#define SDA_HIGH()  (GPEC = (1 << SDA)) 
//#define SCL_LOW()   (GPES = (1 << SCL))
//#define SCL_HIGH()  (GPEC = (1 << SCL))
//#define SDA_READ()  ((GPI & (1 << SDA)) != 0)
//
//void resetRTC() {
//
//  pinMode(SDA, INPUT_PULLUP);
//  pinMode(SCL, INPUT_PULLUP);
//  do {
//    SDA_HIGH();
//    SCL_HIGH();
//    if (SDA_READ()) {
//      SDA_LOW();
//      SDA_HIGH();
//    }
//    SCL_LOW();
//  } while (SDA_READ() == 0);
//
//}
