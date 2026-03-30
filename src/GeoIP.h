// src/GeoIP.h
#ifndef GEOIP_H
#define GEOIP_H

#include <Arduino.h>

void fetchLocationData();
void saveLocationToEEPROM();
void loadLocationFromEEPROM();
bool isLocationValid();

#endif