void actOnSerial () {
  while (Serial.available() > 0) {
    char cmd = Serial.read();
    switch (cmd) {
      case 'u':
        motorUp();
        break;
      case 'd':
        motorDown();
        break;
      case 's':
        motorStop();
        break;
      case 'h':
        help();
        break;
      case 'c':
        calibrate();
        break;
      case 't':
        toggleStats();
        break;
      case 'f':
        toggleStatsFast();
        break;
    }
  }
}

void help () {
  Serial.println("--- COMMANDS ---");
  Serial.println("h : Help, this menu.");
  Serial.println("u : Up, drive garage door up.");
  Serial.println("d : Down, drive garage door down.");
  Serial.println("s : Stop the garage door.");
  Serial.println("c : Calibration.");
  Serial.println("t : Toggle stats.");
  Serial.println("f : Toggle fast stats.");
}

void stats () {
  // print stats every second
  if (showStats == false || abs(millis() - prevStatsTime) < 1000) {
    return;
  }

  prevStatsTime = millis();

  Serial.println("top  \tbottom\tload");
  Serial.print(top);
  Serial.print("\t");
  Serial.print(bottom);
  Serial.print("\t");
  Serial.println(load);

  Serial.println("motorShouldStart\tmotorShouldStop");
  Serial.print(motorShouldStart);
  Serial.print("\t\t\t");
  Serial.println(motorShouldStop);

  Serial.print("running: ");
  Serial.println(running);
  Serial.print("direction: ");
  Serial.println(direction);
  Serial.print("highSpeed: ");
  Serial.println(highSpeed);
  Serial.print("position: ");
  Serial.println(position);
}

void statsFast () {
  // print stats every 100 ms
  if (showStatsFast == false || abs(millis() - prevStatsTime) < 100) {
    return;
  }

  prevStatsTime = millis();

  Serial.println("top  \tbottom\tload");
  Serial.print(top);
  Serial.print("\t");
  Serial.print(bottom);
  Serial.print("\t");
  Serial.println(load);
}

void toggleStats () {
  showStats = !showStats;
}

void toggleStatsFast () {
  showStatsFast = !showStatsFast;
}
