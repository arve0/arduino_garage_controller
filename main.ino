#define LOAD_MEASUREMENTS 10
#define HIGH_LOAD 350
#define HIGH_LOAD_START 900
#define START_TIME 600
#define LOW_SPEED_TIME 2000
#define LOW_SPEED_TIME_END 4000
#define DEBOUNCE_READS 10

#define TOP_PIN A0  // white
#define BOTTOM_PIN A1  // green
#define LOAD_PIN A2  // yellow - PB0, 19 of 28

#define WIRELESS_PIN A3  // blue
#define BUTTON_PIN A4  // white + red

#define UP_PIN 10  // yellow - PA4, 23 of 28
#define DOWN_PIN 11  // green - PA5, 22 of 28
#define HIGH_SPEED_PIN 12  // blue - PA0 + PA3, 27 + 24 of 28

// ---- STATE ----

// motor
int direction = -1;  // -1: down, +1: up
bool running = false;
bool highSpeed = false;
long position = 0;

// control motor
bool motorShouldStart = false;
bool motorShouldStop = false;

// inputs
bool top = false;
bool bottom = false;
bool wireless = false;
bool button = false;
bool tempInput = false;
int buttonReadsEqual = 0;
int wirelessReadsEqual = 0;
int load = 0;

void readInputs () {
  top = digitalRead(TOP_PIN) == 1;
  bottom = digitalRead(BOTTOM_PIN) == 1;
  load = loadRead(LOAD_PIN);

  buttonRead();
  wirelessRead();
}

int loadRead (int pin) {
  long integral = 0;
  for (int i = 0; i < LOAD_MEASUREMENTS; i += 1) {
    integral += analogRead(pin);
    delay(1);
  }
  return integral / LOAD_MEASUREMENTS;
}

void buttonRead () {
  tempInput = digitalRead(BUTTON_PIN) == 0;
  if (tempInput != button) {
    // debounce
    buttonReadsEqual += 1;
    if (buttonReadsEqual > DEBOUNCE_READS) {
      button = tempInput;
      if (button && running) {
        motorShouldStop = true;
      } else if (button && !running) {
        motorShouldStart = true;
      }
    }
  } else {
    buttonReadsEqual = 0;
  }
}

void wirelessRead () {
  tempInput = digitalRead(WIRELESS_PIN) == 0;
  if (tempInput != wireless) {
    // debounce
    wirelessReadsEqual += 1;
    if (wirelessReadsEqual > DEBOUNCE_READS) {
      wireless = tempInput;
      if (!wireless && running) {
        motorShouldStop = true;
      } else if (wireless && !running) {
        motorShouldStart = true;
      }
    }
  } else {
    wirelessReadsEqual = 0;
  }
}

// timestamps
#define UINT_LONG_MAX 4294967295
unsigned long motorStart = 0;
unsigned long motorHighSpeedStart = 0;

unsigned long timeSince (unsigned long timestamp) {
  unsigned long now = millis();
  if (now < timestamp) {
    // millis timer has wrapped
    return UINT_LONG_MAX - timestamp + now;
  }
  return now - timestamp;
}

// stats
unsigned long prevStatsTime = 0;
bool showStats = false;

// calibration
unsigned long totalTimeLowSpeed;
unsigned long totalTimeHighSpeed;
float fastFactor = 2;

// ---- STATE END ----


void setup () {
  Serial.begin(9600);
  help();
  readCalibration();

  pinMode(TOP_PIN, INPUT_PULLUP);
  pinMode(BOTTOM_PIN, INPUT_PULLUP);
  pinMode(WIRELESS_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(UP_PIN, OUTPUT);
  pinMode(DOWN_PIN, OUTPUT);
  pinMode(HIGH_SPEED_PIN, OUTPUT);
}

void loop () {
  readInputs();

  shouldStart();
  shouldChangeSpeed();
  shouldStop();

  updatePosition();

  delay(1);
  stats();
  actOnSerial();
}

void shouldStart () {
  if (running || !motorShouldStart) {
    return;
  }
  if (bottom) {
    position = 0;
    motorUp();
    return;
  }
  if (top) {
    position = totalTimeLowSpeed;
    motorDown();
    return;
  }
  if (direction == 1) {
    motorDown();
    return;
  }
  if (direction == -1) {
    motorUp();
    return;
  }
}

void shouldChangeSpeed () {
  if (!running) {
    return;
  }

  if (direction == -1 && position < LOW_SPEED_TIME_END) {
    if (highSpeed) {
      motorLowSpeed();
    }
    return;
  }

  if (direction == 1 &&
      diffOrZero(totalTimeLowSpeed, position) < LOW_SPEED_TIME_END) {
    if (highSpeed) {
      motorLowSpeed();
    }
    return;
  }

  unsigned long runningTime = timeSince(motorStart);
  if (!highSpeed && runningTime > LOW_SPEED_TIME) {
    motorHighSpeed();
    return;
  }
}

void shouldStop () {
  if (!running) {
    return;
  }
  if (motorShouldStop) {
    motorStop();
    return;
  }
  if (top && direction == 1) {
    motorStop();
    return;
  }
  if (bottom && direction == -1) {
    motorStop();
    return;
  }

  unsigned long runningTime = timeSince(motorStart);
  unsigned long highSpeedTime = timeSince(motorHighSpeedStart);
  if (runningTime < START_TIME && load > HIGH_LOAD_START) {
    stopDueToHighLoad();
    Serial.println("HIGH LOAD START");
    return;
  }
  if (highSpeedTime < START_TIME && load > HIGH_LOAD_START) {
    stopDueToHighLoad();
    Serial.println("HIGH LOAD START");
    return;
  }
  if (runningTime >= START_TIME && highSpeedTime > START_TIME && load > HIGH_LOAD) {
    stopDueToHighLoad();
    return;
  }
}

void stopDueToHighLoad () {
  // TODO: reverse();
  motorStop();
  Serial.print("HIGH LOAD: ");
  Serial.println(load);
  stats();
}

unsigned long lastPosTime = 0;
void updatePosition () {
  if (!running || lastPosTime == 0) {
    lastPosTime = millis();
    return;
  }
  unsigned long diff = timeSince(lastPosTime);

  int multiplier = highSpeed ? fastFactor : 1;
  multiplier *= direction;  // plus up, minus down

  position += multiplier * diff;

  lastPosTime = millis();
}

unsigned long diffOrZero (unsigned long a, long b) {
  if (b > a) {
    return 0;
  }
  return a - b;
}
