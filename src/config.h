#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define PROJECT_NAME        "Projecteur X2"
#define PROJECT_VERSION     "1.3.0"
#define AP_SSID             "Projecteur-X2"
#define AP_PASSWORD         "12345678"
#define CONFIG_VERSION      "v13"

#define PIR_PIN             D5
#define RELAY_PIN           D6
#define BUTTON_MANUAL       D7

// Variables globales
extern char cityValue[40];
extern char latitudeValue[12];
extern char longitudeValue[12];
extern char timezoneValue[40];

extern bool isNight;
extern bool manualOverride;

void loadLocationFromEEPROM();
void saveLocationToEEPROM();

#endif