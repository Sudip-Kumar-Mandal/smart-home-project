#include <time.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
struct tm timeinfo;

void printLocalTime() {
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time from NTP server");
        return;
    }
    Serial.print("Current local time: ");
    Serial.print(asctime(&timeinfo));
}

void setup_time() {
    configTime(gmtOffset_sec, 0, ntpServer);
    Serial.print("Configuring time...");
    delay(5000);
    printLocalTime();
}

int getTodayTime() {
    int todayTime = 0;
    if (!getLocalTime(&timeinfo)) {
        return 0;
    }

    todayTime = timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;
    return todayTime;
}