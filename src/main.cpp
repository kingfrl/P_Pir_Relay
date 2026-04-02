#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <IotWebConf.h>
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

// IotWebConf 2.3.3 - Correct constructor
DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(PROJECT_NAME, &dnsServer, &server, AP_PASSWORD, CONFIG_VERSION);

IotWebConfParameter cityParam("Ville", "city", cityValue, sizeof(cityValue));
IotWebConfParameter latitudeParam("Latitude", "lat", latitudeValue, sizeof(latitudeValue));
IotWebConfParameter longitudeParam("Longitude", "lon", longitudeValue, sizeof(longitudeValue));
IotWebConfParameter timezoneParam("Fuseau horaire", "tz", timezoneValue, sizeof(timezoneValue));

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n=== " PROJECT_NAME " v" PROJECT_VERSION " démarrage ===");

    iotWebConf.addParameter(&cityParam);
    iotWebConf.addParameter(&latitudeParam);
    iotWebConf.addParameter(&longitudeParam);
    iotWebConf.addParameter(&timezoneParam);

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