
void sensors_measureWeight() {

  currentTime = rtc.now();
  lastMeasuredWeight = scale.get_units(1);
  
  lastFilteredWeight = filteredWeight;
  medianFilteredWeight.add((int)lastMeasuredWeight);
  filteredWeight = medianFilteredWeight.getMedian();
  
  medianFilterInterval.add(millis() - sensorsLastReadTime);
  measurementInterval = medianFilterInterval.getMedian();

  lastMeasuredCurrent = emon1.calcIrms(1480)*230;
  medianCurrent.add(lastMeasuredCurrent);
  filteredCurrent = medianCurrent.getMedian();

  if (useProximitySensor) {
    lastMeasuredAmbientLight = vcnl.readAmbient();
    lastMeasuredProximity = vcnl.readProximity();
  }

  rtc_serialPrintTime(currentTime, false);

  Serial.print(millis());
  Serial.print("\t| measured:\t");
  Serial.print(lastMeasuredWeight, 1);
  Serial.print("g\t| filtered:\t");
  Serial.print(filteredWeight);
  Serial.print("g\t| power:\t");
  Serial.print(lastMeasuredCurrent);
  Serial.print("\t| ambient:\t ");
  Serial.print(lastMeasuredAmbientLight);
  Serial.print("\t| Proximity:\t");
  Serial.println(lastMeasuredProximity);  
}

void sensors_logWeight() {
  sd_prepareFilenames(currentTime, todayFilenameRegular, todayFilenameChange, todayFilenameUsers);
  sd_prepareTimeString(currentTime, timeString);
  sd_prepareDateString(currentTime, dateString);

  sd_logData(dateString, timeString, lastMeasuredWeight, lastMeasuredCurrent, lastMeasuredAmbientLight, lastMeasuredProximity);
  sd_logRegularValue();
  sd_logChangeValue();

  if ((int)filteredWeight != (int)lastFilteredWeight) {
    lastMeasuredWeight = filteredWeight;
  }  
}


void sensors_senseHeaterState() {
  if ((filteredCurrent > HEATER_CURRENT_THRESHOLD) && !heaterRunning) {
    heaterStartedTime = rtc.now();
    sd_prepareTimeString(heaterStartedTime, heaterTimeString);
    heaterRunning = true;
  }
  else if ((filteredCurrent < HEATER_CURRENT_THRESHOLD) && heaterRunning) {
    heaterStoppedTime = rtc.now();
    sd_prepareTimeString(heaterStoppedTime, heaterTimeString);
    heaterRunning = false;
    scale.tare();
  }
}
