#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] Connected from %s\n", num, webSocket.remoteIP(num).toString().c_str());
      break;
    case WStype_TEXT:
      String data = String((char*)payload);
      Serial.printf("[%u] Received text: %s\n", num, payload);

      if (data.startsWith("bedRoomLightingMode")) {
        changeBedRoomLightingMode(data.substring(String("bedRoomLightingMode").length()).toInt());
        String jsonString = "{\"bedRoomLightingMode\": " + String(bedRoomLightingMode) + "}";
        webSocket.broadcastTXT(jsonString);
      }
      else if (data.startsWith("bedRoomLightSchedule")) {
        char *token = strtok(const_cast<char*>(data.substring(String("bedRoomLightSchedule").length()).c_str()), ",");
    
        int i = 0;
        while (token != NULL) {
          bedRoomLightScheduleArray[i] = atoi(token);
          if(nextScheduledTimeIndex == -1 && bedRoomLightScheduleArray[i] < getTodayTime()) {
            nextScheduledTimeIndex = i;
          }
          token = strtok(NULL, ",");
          i++;
        }

        for(int j = 0; j < i; j++) {
          Serial.print(String(bedRoomLightScheduleArray[j]) + " ");
        }

        bedRoomLightScheduleArraySize = i;
        
        bedRoomLightScheduleArray[bedRoomLightScheduleArraySize] = INT_MAX;
      }
      else if (data.startsWith("bedRoomLight")) {
        displayBedRoomLight(data.substring(String("bedRoomLight").length()).toInt());
        String jsonString = "{\"bedRoomLight\": " + String(isBedRoomLightOn) + "}";
        webSocket.broadcastTXT(jsonString);
      }
      else if (data.startsWith("livingRoomLight")) {
        displayLivingRoomLight(data.substring(String("livingRoomLight").length()).toInt());
        String jsonString = "{\"livingRoomLight\": " + String(isLivingRoomLightOn) + "}";
        webSocket.broadcastTXT(jsonString);
      }
      
      break;
  }
}

void writeBedRoomLightIfTouchedWebSocket() {
  if(isBedRoomLightTouched()) {
    isBedRoomLightOn = !isBedRoomLightOn;
    digitalWrite(bedRoomLightPin, isBedRoomLightOn);
    String jsonString = "{\"bedRoomLight\": " + String(isBedRoomLightOn) + "}";
    webSocket.broadcastTXT(jsonString);
  }
}

void writeLivingRoomLightIfTouchedWebSocket() {
  if(isLivingRoomLightTouched()) {
    isLivingRoomLightOn = !isLivingRoomLightOn;
    digitalWrite(livingRoomLightPin, isLivingRoomLightOn);
    String jsonString = "{\"livingRoomLight\": " + String(isLivingRoomLightOn) + "}";
    webSocket.broadcastTXT(jsonString);
  }
}

void writeEnvironmentDataWebSocket() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if(isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to real from DHT11");
    return;
  }

  String jsonString = "{\"environment\": [" + String(temperature) + "," + String(humidity) + "]}";
  webSocket.broadcastTXT(jsonString);
}

void writeBedRoomLightIfOccupiedWebSocket() {
  if(isBedRoomOccupied() != isBedRoomLightOn) {
    isBedRoomLightOn = !isBedRoomLightOn;
    digitalWrite(bedRoomLightPin, isBedRoomLightOn);
    String jsonString = "{\"bedRoomLight\": " + String(isBedRoomLightOn) + "}";
    webSocket.broadcastTXT(jsonString);
  }
}

void writeBedRoomLightIfScheduledWebSocket() {
  int time = getTodayTime();
  if(
    time > bedRoomLightScheduleArray[nextScheduledTimeIndex] && 
    time < bedRoomLightScheduleArray[nextScheduledTimeIndex + 1]
    ) {
    isBedRoomLightOn = (nextScheduledTimeIndex % 2 == 0);
    digitalWrite(bedRoomLightPin, isBedRoomLightOn);
    String jsonString = "{\"bedRoomLight\": " + String(isBedRoomLightOn) + "}";
    webSocket.broadcastTXT(jsonString);
    nextScheduledTimeIndex = (nextScheduledTimeIndex + 1) % bedRoomLightScheduleArraySize;
  }
}