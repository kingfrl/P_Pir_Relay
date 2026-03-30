#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include <IotWebConf.h>

extern IotWebConf iotWebConf;
extern IotWebConfParameterGroup locationGroup;
extern IotWebConfSelectParameter cityParam;

void setupWiFi();
void handleWiFi();
void startAPMode();
void connectToWiFi(const char* ssid, const char* password);

#endif