// src/time_utils.h
#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>

extern bool isNight;           // Variable globale mise à jour dans le loop
extern double sunriseHour;
extern double sunsetHour;

void setupTimeUtils();
void handleTimeUtils();        // À appeler dans loop()

bool isCurrentlyNight();
String getCurrentTimeFormatted();

#endif