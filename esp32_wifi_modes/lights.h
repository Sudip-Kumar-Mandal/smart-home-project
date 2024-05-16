const int touchThreshold = 40;

const int bedRoomLightPin = 18;
const int bedRoomLightTouchPin = 13;
bool isBedRoomLightOn = false;
bool isBedRoomLightTouchSafe = true;

int bedRoomLightingMode = 0;
bool isBedRoomLightingModeChanged = false;

const int livingRoomLightPin = 19;
const int livingRoomLightTouchPin = 4;
bool isLivingRoomLightOn = false;
bool isLivingRoomLightTouchSafe = true;

int bedRoomLightScheduleArray[100];
int bedRoomLightScheduleArraySize = 0;
int nextScheduledTimeIndex = -1;

void setup_lights() {
  pinMode(bedRoomLightPin, OUTPUT);
  pinMode(livingRoomLightPin, OUTPUT);
}

void displayBedRoomLight(bool state) {
  isBedRoomLightOn = state;
  Serial.println("Bed room light: " + String(isBedRoomLightOn));
  digitalWrite(bedRoomLightPin, isBedRoomLightOn);
}

void displayLivingRoomLight(bool state) {
  isLivingRoomLightOn = state;
  Serial.println("Living room light: " + String(isLivingRoomLightOn));
  digitalWrite(livingRoomLightPin, isLivingRoomLightOn);
}

void changeBedRoomLightingMode(int mode) {
  isBedRoomLightingModeChanged = true;
  bedRoomLightingMode = mode;
  Serial.println("Bed room lighting mode: " + String(bedRoomLightingMode));
}

void changeBedRoomLightSchedule() {

}

bool isBedRoomLightTouched() {
  if((touchRead(bedRoomLightTouchPin) < touchThreshold) && isBedRoomLightTouchSafe) {
    isBedRoomLightTouchSafe = false;
    return true;
  }
  if((touchRead(bedRoomLightTouchPin) > touchThreshold)) {
    isBedRoomLightTouchSafe = true;
  }
  return false;
}

bool isLivingRoomLightTouched() {
  if((touchRead(livingRoomLightTouchPin) < touchThreshold) && isLivingRoomLightTouchSafe) {
    isLivingRoomLightTouchSafe = false;
    return true;
  }
  if((touchRead(livingRoomLightTouchPin) > touchThreshold)) {
    isLivingRoomLightTouchSafe = true;
  }
  return false;
}
