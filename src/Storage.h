#pragma once
#include <Arduino.h>
#include <String>

void initFileSystem();
String loadHTMLFile(String path);

// Gestion WiFi
void saveCredentials(String ssid, String pass);
bool loadCredentials(String &ssid, String &pass);
void deleteCredentials();

// Gestion Localisation
void saveLocation(float lat, float lon, int offset);
bool loadLocation(float &lat, float &lon, int &offset);
void deleteLocation();

// Gestion Mode Jour / Nuit
void saveDayNightState(bool enabled);
bool loadDayNightState(bool &enabled);