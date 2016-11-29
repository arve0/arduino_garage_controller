
void motorUp () {
  digitalWrite(DOWN_PIN, LOW);
  digitalWrite(UP_PIN, HIGH);

  direction = 1;
  running = true;
  motorStart = millis();
  motorShouldStart = false;
  Serial.println("UP");
}

void motorDown () {
  digitalWrite(UP_PIN, LOW);
  digitalWrite(DOWN_PIN, HIGH);

  direction = -1;
  running = true;
  motorStart = millis();
  motorShouldStart = false;
  Serial.println("DOWN");
}

void motorHighSpeed () {
  digitalWrite(HIGH_SPEED_PIN, HIGH);
  highSpeed = true;
  motorHighSpeedStart = millis();
  Serial.println("HIGH SPEED");
}

void motorLowSpeed () {
  digitalWrite(HIGH_SPEED_PIN, LOW);
  highSpeed = false;
  motorHighSpeedStart = 0;
  Serial.println("LOW SPEED");
}

void motorStop () {
  digitalWrite(UP_PIN, LOW);
  digitalWrite(DOWN_PIN, LOW);
  digitalWrite(HIGH_SPEED_PIN, LOW);

  running = false;
  motorShouldStop = false;
  motorShouldStart = false;
  highSpeed = false;
  motorStart = 0;
  motorHighSpeedStart = 0;
  Serial.println("STOP");
}
