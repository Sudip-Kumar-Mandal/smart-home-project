#include <WiFi.h>
#include "lights.h"
#include "dht.h"
#include "ctime.h"
#include "display.h"
#include "occupancy.h"
#include "firebase.h"
#include "server.h"
#include "websocket.h"

#define WIFI_SSID "HomeWiFi"
#define WIFI_PASSWORD "88888888"

#define AP_SSID "ESP32_SmartHome"
#define AP_PASSWORD "123456789"

#define INT_MAX 2147483647

IPAddress local_ip(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

bool isConnectedToHomeWifi = false;

unsigned long lastWiFiStatusCheckTime = 0;
const int WiFiStatusCheckDelay = 30000;

void setup_serial() {
  Serial.begin(115200);
}

void setup_wifi_sta() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  isConnectedToHomeWifi = true;
}

void setup_wifi_ap() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

void setup() {
  setup_serial();
  setup_lights();
  setup_ir_pins();
  setup_display();
  setup_wifi_sta();
  setup_time();
  setup_firebase();
  setup_firebase_input_streams();
  dht.begin();
  delay(2000);
}



void loop() {
  if(isConnectedToHomeWifi) {
    // station mode

    if(Firebase.ready()) {
      readStreamBedRoomLight();
      readStreamLivingRoomLight();
      readStreamBedRoomLightingMode();

      if(bedRoomLightingMode == 0) {
        writeBedRoomLightIfTouched();
      }
      if(bedRoomLightingMode == 1) {            // automatic lighting based on occupancy detection
        writeBedRoomLightIfOccupied();
      }
      else if(bedRoomLightingMode == 2) {       // scheduled lighting
        readStreamBedRoomLightSchedule();
        writeBedRoomLightIfTouched();
      }

      if(bedRoomLightingMode != 1 && isBedRoomLightingModeChanged) {            // swith only
        sevenSegmentDisplay(10);                                                // off seven segment
        digitalWrite(bedRoomLightPin, isBedRoomLightOn);
        isBedRoomLightingModeChanged = false;
      }
      if(bedRoomLightingMode == 1 && isBedRoomLightingModeChanged) {            // automatic based on occupancy detection
        sevenSegmentDisplay(countPersonsInRoom);
        isBedRoomLightingModeChanged = false;
      }

      writeLivingRoomLightIfTouched();

      if(millis() - prevMillis >= 15000) {
        Serial.printf("Time: %d\n", getTodayTime());
        if(bedRoomLightingMode == 2) {
          writeBedRoomLightIfScheduled(); 
        }
        writeEnvironmentData();
        prevMillis = millis();
      }
    }
  }
  else {
    // soft ap mode

    if(bedRoomLightingMode == 0) {
      writeBedRoomLightIfTouchedWebSocket();
    }
    if(bedRoomLightingMode == 1) {            // automatic lighting based on occupancy detection
      writeBedRoomLightIfOccupiedWebSocket();
    }
    else if(bedRoomLightingMode == 2) {       // scheduled lighting
      writeBedRoomLightIfTouchedWebSocket();
    }

    if(bedRoomLightingMode != 1 && isBedRoomLightingModeChanged) {            // swith only
      sevenSegmentDisplay(10);                                                // off seven segment
      digitalWrite(bedRoomLightPin, isBedRoomLightOn);
      isBedRoomLightingModeChanged = false;
    }
    if(bedRoomLightingMode == 1 && isBedRoomLightingModeChanged) {            // automatic based on occupancy detection
      sevenSegmentDisplay(countPersonsInRoom);
      isBedRoomLightingModeChanged = false;
    }

    writeLivingRoomLightIfTouchedWebSocket();

    if(millis() - prevMillis >= 15000) {
      Serial.printf("Time: %d\n", getTodayTime());
      if(bedRoomLightingMode == 2) {
        writeBedRoomLightIfScheduledWebSocket(); 
      }
      writeEnvironmentDataWebSocket();
      prevMillis = millis();
    }
  
    webSocket.loop();
  }

  if(millis() - lastWiFiStatusCheckTime >= WiFiStatusCheckDelay) {
    lastWiFiStatusCheckTime = millis();
    if(isConnectedToHomeWifi) {
      Serial.println("In STA mode");
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Disconnected from home WiFi");
        isConnectedToHomeWifi = false;

        setup_wifi_ap();
        Serial.println("WiFi running in AP mode");
        setup_spiffs();
        delay(1000);
        Serial.println("Spiffs set up");
        setup_async_server();
        delay(1000);
        Serial.println("Async server set up");

        server.begin();
        webSocket.begin();
        webSocket.onEvent(onWebSocketEvent);
      }
    }
    else {
      Serial.println("In AP mode");
      int networksFound = WiFi.scanNetworks();
      if (networksFound > 0) {
        for (int i = 0; i < networksFound; ++i) {
          if (WiFi.SSID(i) == WIFI_SSID) {
            Serial.println("Home WiFi network found!, restarting");
            String jsonString = "{\"firebase\": 1}";
            webSocket.broadcastTXT(jsonString);
            delay(2000);
            ESP.restart();
          }
        }
      }
    }
  }
}