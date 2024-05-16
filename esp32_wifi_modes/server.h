#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

AsyncWebServer server(80);

void setup_spiffs() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }
}


void setup_async_server() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.css", "text/css");
  });

  server.on("/chart.js", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send(SPIFFS, "/chart.js", "text/javascript");
  });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send(SPIFFS, "/index.js", "text/javascript");
  });

  server.on("/site.webmanifest", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/site.webmanifest");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/favicon.ico");
  });
  server.on("/android-chrome-192x192.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/android-chrome-192x192.png");
  });
  server.on("/android-chrome-512x512.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/android-chrome-512x512.png");
  });
  server.on("/apple-touch-icon.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/apple-touch-icon.png");
  });
  server.on("/favicon-16x16.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon-16x16.png");
  });
  server.on("/favicon-32x32.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon-32x32.png");
  });
  
}