#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>

#define API_KEY "AIzaSyCwG-YU1IqFH4nR5b0RC1CWeX5v_ASptrQ"
#define DATABASE_URL "https://esp32-smart-home-project-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define USER_EMAIL "user123@gmail.com"
#define USER_PASSWORD ":("

FirebaseData bedRoomLightStream;
FirebaseData bedRoomLightScheduleStream;
FirebaseData bedRoomLightingModeStream;

FirebaseData livingRoomLightStream;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int prevMillis = 0;

void setup_firebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void setup_firebase_input_streams() {
  Firebase.RTDB.beginStream(&bedRoomLightStream, "home/bedRoom/isLightOn");
  Firebase.RTDB.beginStream(&bedRoomLightScheduleStream, "home/bedRoom/lightSchedule");
  Firebase.RTDB.beginStream(&bedRoomLightingModeStream, "home/bedRoom/lightingMode");
  Firebase.RTDB.beginStream(&livingRoomLightStream, "home/livingRoom/isLightOn");
}

void end_firebase_input_streams() {
  Firebase.RTDB.endStream(&bedRoomLightStream);
  Firebase.RTDB.endStream(&bedRoomLightScheduleStream);
  Firebase.RTDB.endStream(&bedRoomLightingModeStream);
  Firebase.RTDB.endStream(&livingRoomLightStream);
}

void readStreamBedRoomLight() {
  Firebase.RTDB.readStream(&bedRoomLightStream);
  if(bedRoomLightStream.streamAvailable()) {
    if(bedRoomLightStream.dataType() == "boolean") {
      displayBedRoomLight(bedRoomLightStream.boolData());
    }
  }
}

void readStreamLivingRoomLight() {
  Firebase.RTDB.readStream(&livingRoomLightStream);
  if(livingRoomLightStream.streamAvailable()) {
    if(livingRoomLightStream.dataType() == "boolean") {
      displayLivingRoomLight(livingRoomLightStream.boolData());
    }
  }
}

void readStreamBedRoomLightingMode() {
  Firebase.RTDB.readStream(&bedRoomLightingModeStream);
  if(bedRoomLightingModeStream.streamAvailable()) {
    if(bedRoomLightingModeStream.dataType() == "int") {
      changeBedRoomLightingMode(bedRoomLightingModeStream.intData());
    }
  }
}

void readStreamBedRoomLightSchedule() {
  Firebase.RTDB.readStream(&bedRoomLightScheduleStream);
  if(bedRoomLightScheduleStream.streamAvailable()) {
    if(bedRoomLightScheduleStream.dataType() == "array") {
      FirebaseJsonArray &arr = bedRoomLightScheduleStream.jsonArray();
      FirebaseJsonData result;

      bedRoomLightScheduleArraySize = arr.size();
      for(int i = 0; i < arr.size(); i++) {
        arr.get(result, i);
        Serial.println(result.intValue);
        bedRoomLightScheduleArray[i] = result.intValue;
        if(nextScheduledTimeIndex == -1 && bedRoomLightScheduleArray[i] < getTodayTime()) {
          nextScheduledTimeIndex = i;
        }
      }
      bedRoomLightScheduleArray[bedRoomLightScheduleArraySize] = INT_MAX;
    }
  }
}

void writeBedRoomLightIfTouched() {
  if(isBedRoomLightTouched()) {
    isBedRoomLightOn = !isBedRoomLightOn;
    digitalWrite(bedRoomLightPin, isBedRoomLightOn);
    Firebase.RTDB.setBool(&fbdo, "home/bedRoom/isLightOn", isBedRoomLightOn);
  }
}

void writeLivingRoomLightIfTouched() {
  if(isLivingRoomLightTouched()) {
    isLivingRoomLightOn = !isLivingRoomLightOn;
    digitalWrite(livingRoomLightPin, isLivingRoomLightOn);
    Firebase.RTDB.setBool(&fbdo, "home/livingRoom/isLightOn", isLivingRoomLightOn);
  }
}

void writeBedRoomLightIfOccupied() {
  if(isBedRoomOccupied() != isBedRoomLightOn) {
    isBedRoomLightOn = !isBedRoomLightOn;
    digitalWrite(bedRoomLightPin, isBedRoomLightOn);
    Firebase.RTDB.setBool(&fbdo, "home/bedRoom/isLightOn", isBedRoomLightOn);
  }
}

void writeBedRoomLightIfScheduled() {
  int time = getTodayTime();
  if(
    time > bedRoomLightScheduleArray[nextScheduledTimeIndex] && 
    time < bedRoomLightScheduleArray[nextScheduledTimeIndex + 1]
    ) {
    isBedRoomLightOn = (nextScheduledTimeIndex % 2 == 0);
    digitalWrite(bedRoomLightPin, isBedRoomLightOn);
    Firebase.RTDB.setBool(&fbdo, "home/bedRoom/isLightOn", isBedRoomLightOn);
    nextScheduledTimeIndex = (nextScheduledTimeIndex + 1) % bedRoomLightScheduleArraySize;
  }
}

void writeEnvironmentData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if(isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to real from DHT11");
    return;
  }

  FirebaseJson jsonEnvironment;
  jsonEnvironment.set("/temperature", temperature);
  jsonEnvironment.set("/humidity", humidity);

  Firebase.RTDB.setJSON(&fbdo, "home/environment", &jsonEnvironment);
}