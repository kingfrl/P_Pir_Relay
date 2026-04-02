#include "wifi_manager.h"
#include <ESP8266WebServer.h>
#include <DNSServer.h>

DNSServer dnsServer;
ESP8266WebServer server(80);

const char wifiInitialApPassword[] = "12345678";
const char thingName[] = "Projecteur-X2";

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

// Variables globales pour stocker les informations de localisation
char latitudeValue[16] = "";
char longitudeValue[16] = "";
char timezoneValue[40] = "";
char cityValue[40] = "";

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

void displayConfigPage() {
    String html = "<html><head><title>Configuration</title></head><body>";
    html += "<h1>Configuration de la Localisation</h1>";
    html += "<form action='/config' method='post'>";
    html += "<label for='city'>Ville:</label>";
    html += "<input type='text' id='city' name='city' value='" + String(cityValue) + "' readonly><br><br>";
    html += "<label for='latitude'>Latitude:</label>";
    html += "<input type='text' id='latitude' name='latitude' value='" + String(latitudeValue) + "' readonly><br><br>";
    html += "<label for='longitude'>Longitude:</label>";
    html += "<input type='text' id='longitude' name='longitude' value='" + String(longitudeValue) + "' readonly><br><br>";
    html += "<label for='timezone'>Fuseau Horaire:</label>";
    html += "<input type='text' id='timezone' name='timezone' value='" + String(timezoneValue) + "' readonly><br><br>";
    html += "<input type='submit' value='Sauvegarder'>";
    html += "</form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void saveConfig() {
    if (server.hasArg("city") && server.hasArg("latitude") && server.hasArg("longitude") && server.hasArg("timezone")) {
        strncpy(cityValue, server.arg("city").c_str(), 39);
        cityValue[39] = '\0';
        strncpy(latitudeValue, server.arg("latitude").c_str(), 15);
        latitudeValue[15] = '\0';
        strncpy(longitudeValue, server.arg("longitude").c_str(), 15);
        longitudeValue[15] = '\0';
        strncpy(timezoneValue, server.arg("timezone").c_str(), 39);
        timezoneValue[39] = '\0';
    }
    server.sendHeader("Location", "/config");
    server.send(303);
}

void handleRoot() {
    String html = "<html><body>";
    html += "<h1>Projecteur X2</h1>";
    html += "<p>Statut : Connected</p>";
    html += "<p>Adresse IP: " + WiFi.localIP().toString() + "</p>";
    html += "<p><a href='/config'>Configurer la localisation</a></p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void setupWiFi() {
    iotWebConf.init();
    server.on("/", handleRoot);
    server.on("/config", displayConfigPage);
    server.onNotFound([](){ iotWebConf.handleNotFound(); });
}

void handleWiFi() {
    iotWebConf.doLoop();
}