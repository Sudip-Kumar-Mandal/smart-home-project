const int durationOneSensorMin = 10;
const int durationOneSensorMax = 1000;
const int durationBetweenSensorsMin = -10;
const int durationBetweenSensorsMax = 1000;

const int irSensorPins[2] = {34, 35};
bool irSensorStates[2] = {false, false};
unsigned long startTimes[2] = {0, 0};
unsigned long endTimes[2] = {0, 0};

int countPersonsInRoom = 0;

void setup_ir_pins() {
    pinMode(irSensorPins[0], INPUT);
    pinMode(irSensorPins[1], INPUT);
}

bool readIrPin(int pin) {
  if(digitalRead(pin) == HIGH)
    return false;
  else
    return true;
}

bool validateIrDuration(int start, int end, int min, int max) {
  if(end - start >= min && end - start <= max) {
    return true;
  }
  else {
    return false;
  }
}

void resetIrTimes() {
  startTimes[0] = 0;
  startTimes[1] = 0;
  endTimes[0] = 0;
  endTimes[1] = 0;
}

bool isBedRoomOccupied() {
  if(readIrPin(irSensorPins[0]) && !irSensorStates[0]) {
    startTimes[0] = millis();
    irSensorStates[0] = true;
  }
  
  if(!readIrPin(irSensorPins[0]) && irSensorStates[0]) {
    endTimes[0] = millis();
    irSensorStates[0] = false;
  }
      
  if(readIrPin(irSensorPins[1]) && !irSensorStates[1]) {
    startTimes[1] = millis();
    irSensorStates[1] = true;
  }
  
  if(!readIrPin(irSensorPins[1]) && irSensorStates[1]) {
    endTimes[1] = millis();
    irSensorStates[1] = false;
  }
  
  if(validateIrDuration(startTimes[0], endTimes[0], durationOneSensorMin, durationOneSensorMax) &&
     validateIrDuration(startTimes[1], endTimes[1], durationOneSensorMin, durationOneSensorMax)) {
    if(validateIrDuration(endTimes[0], startTimes[1], durationBetweenSensorsMin, durationBetweenSensorsMax)) {
      
      if(countPersonsInRoom < 9) {
        countPersonsInRoom++;
        Serial.printf("%d\n", countPersonsInRoom);
        sevenSegmentDisplay(countPersonsInRoom);
      }
      resetIrTimes();
    }
    else if(validateIrDuration(endTimes[1], startTimes[0], durationBetweenSensorsMin, durationBetweenSensorsMax)) {

      if(countPersonsInRoom > 0) {
        countPersonsInRoom--;
        Serial.printf("%d\n", countPersonsInRoom);
        sevenSegmentDisplay(countPersonsInRoom);
      }
      resetIrTimes();
    }
  }

  if(countPersonsInRoom > 0) {
    return true;
  }
  else {
    return false;
  }
  
  delay(10);
}