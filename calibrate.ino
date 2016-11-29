
void calibrate () {
  Serial.println("CALIBRATING");
  gotoBottom();
  if (!bottom) {
    Serial.println("CALIBRATION FAILED");
    return;
  }

  // totalTimeLowSpeed
  unsigned long start = millis();
  motorUp();
  while (running) {
    readInputs();
    shouldStop();
    delay(1);
  }
  if (!top) {
    Serial.println("CALIBRATION FAILED");
    return;
  }
  totalTimeLowSpeed = timeSince(start);
  delay(200);

  Serial.print("LOW SPEED TIME: ");
  Serial.println(totalTimeLowSpeed);

  // totalTimeHighSpeed
  start = millis();
  motorHighSpeed();
  motorDown();
  while (running) {
    readInputs();
    shouldStop();
    delay(1);
  }
  if (!bottom) {
    Serial.println("CALIBRATION FAILED");
    return;
  }
  totalTimeHighSpeed = timeSince(start);

  Serial.print("HIGH SPEED TIME: ");
  Serial.println(totalTimeHighSpeed);

  EEPROMWriteLong(0, 0);
  EEPROMWriteLong(1, totalTimeLowSpeed);
  EEPROMWriteLong(2, totalTimeHighSpeed);
}

void readCalibration () {
  // Each byte in EEPROM is 255 as default, when not written to
  if (EEPROMReadLong(0) != 0) {
    // time not read, abort
    Serial.println("NO TIME FOUND");
    return;
  }
  totalTimeLowSpeed = EEPROMReadLong(1);
  totalTimeHighSpeed = EEPROMReadLong(2);
  fastFactor = float(totalTimeLowSpeed) / totalTimeHighSpeed;

  Serial.print("LOW SPEED TIME: ");
  Serial.println(totalTimeLowSpeed);

  Serial.print("HIGH SPEED TIME: ");
  Serial.println(totalTimeHighSpeed);

  Serial.print("HIGH SPEED FACTOR: ");
  Serial.println(fastFactor);
}

void gotoBottom () {
  if (bottom) {
    return;
  }
  motorHighSpeed();
  motorDown();
  do {
    readInputs();
    shouldStop();
    delay(1);
  } while (running);
  delay(200); // do not turn immediately, avoiding high load
}
