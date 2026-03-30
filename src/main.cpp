#include <Arduino.h>
#include <ESP8266WiFi.h>        // ← AJOUT IMPORTANT
#include "config.h"
#include "wifi_manager.h"
#include "ota_handler.h"
#include "pir_sensor.h"
#include "relay_control.h"
#include "mqtt_client.h"
#include "web_interface.h"
#include "time_utils.h"
#include "GeoIP.h"

// Variables globales
char cityValue[40] = "";
char latitudeValue[12] = "48.8566";
char longitudeValue[12] = "2.3522";
char timezoneValue[40] = "Europe/Paris";

bool isNight = false;
bool manualOverride = false;

// Création de l'objet IotWebConf
IotWebConf iotWebConf(PROJECT_NAME, nullptr, nullptr, AP_SSID, AP_PASSWORD);

StringParameter cityParam("Ville", "city", cityValue, sizeof(cityValue), "Paris");
StringParameter latitudeParam("Latitude", "lat", latitudeValue, sizeof(latitudeValue), "48.8566");
StringParameter longitudeParam("Longitude", "lon", longitudeValue, sizeof(longitudeValue), "2.3522");
StringParameter timezoneParam("Fuseau horaire", "tz", timezoneValue, sizeof(timezoneValue), "Europe/Paris");

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n=== " PROJECT_NAME " v" PROJECT_VERSION " démarrage ===");

    loadLocationFromEEPROM();

    setupWiFi();
    setupTimeUtils();
    setupOTA();
    setupPIR();
    setupRelay();
    setupMQTT();
    setupWebInterface();

    Serial.println("✅ Initialisation terminée !");
}

void loop() {
    handleWiFi();
    handleOTA();
    handlePIR();
    handleRelay();
    handleMQTT();
    handleWebInterface();
    handleTimeUtils();
}