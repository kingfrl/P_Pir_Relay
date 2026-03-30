#include "wifi_manager.h"
#include "config.h"
#include "GeoIP.h"
#include <IotWebConf.h>

IotWebConf iotWebConf(PROJECT_NAME, AP_SSID, AP_PASSWORD, CONFIG_VERSION);

void setupWiFi() {
    iotWebConf.addParameter(&cityParam);
    iotWebConf.addParameter(&latitudeParam);
    iotWebConf.addParameter(&longitudeParam);
    iotWebConf.addParameter(&timezoneParam);

    iotWebConf.setConfigSavedCallback([]() { saveConfig(); });
    iotWebConf.setWifiConnectionCallback([]() {
        Serial.println("WiFi connecté !");
        fetchLocationData();
    });

    iotWebConf.setupConfig();
    iotWebConf.start();
}

void handleWiFi() {
    iotWebConf.doLoop();
}