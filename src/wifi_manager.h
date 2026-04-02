#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include <IotWebConf.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

extern IotWebConf iotWebConf;
extern char latitudeValue[];
extern char longitudeValue[];
extern char timezoneValue[];
extern char cityValue[];

void setupWiFi();
void handleWiFi();
void fetchLocationData();
void displayConfigPage();
void saveConfig();

#endif