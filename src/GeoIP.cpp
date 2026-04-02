#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "wifi_manager.h"

void fetchLocationData() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, "http://ip-api.com/json/");

        int httpCode = http.GET();
        if (httpCode > 0) {
            String payload = http.getString();
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);

            const char* status = doc["status"];
            if (strcmp(status, "success") == 0) {
                const char* city = doc["city"];
                double lat = doc["lat"];
                double lon = doc["lon"];
                const char* timezone = doc["timezone"];

                dtostrf(lat, 6, 4, latitudeValue);
                dtostrf(lon, 6, 4, longitudeValue);
                strncpy(timezoneValue, timezone, 39);
                timezoneValue[39] = '\0';
                strncpy(cityValue, city, 39);
                cityValue[39] = '\0';
            }
        }
        http.end();
    }
}