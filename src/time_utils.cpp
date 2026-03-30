/**
 * =============================================
 * time_utils.cpp - Gestion du temps, NTP et lever/coucher du soleil
 * =============================================
 */

#include "time_utils.h"
#include "config.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SolarCalculator.h>
#include <TZ.h>
#include <time.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);  // Mise à jour toutes les 60s

SolarCalculator solarCalc;

double sunriseHour = 6.0;
double sunsetHour  = 22.0;

unsigned long lastSunCalc = 0;
unsigned long lastTimeUpdate = 0;

// =============================================

void setupTimeUtils() {
    Serial.println("Initialisation Time & Solar...");

    timeClient.begin();
    
    // Configuration initiale SolarCalculator avec valeurs par défaut
    double lat = atof(latitudeValue);
    double lon = atof(longitudeValue);
    solarCalc.setPosition(lat, lon);

    // Configuration NTP + Timezone (meilleure méthode pour ESP8266)
    configTime(0, 0, "pool.ntp.org");           // On récupère l'heure UTC
    setenv("TZ", timezoneValue, 1);             // Applique le fuseau horaire
    tzset();

    // Premier calcul du lever/coucher
    calculateSunTimes();
    
    Serial.printf("Position : %.4f, %.4f | TZ: %s\n", lat, lon, timezoneValue);
}

// =============================================

void calculateSunTimes() {
    time_t now = timeClient.getEpochTime();
    struct tm *timeinfo = localtime(&now);

    int year  = timeinfo->tm_year + 1900;
    int month = timeinfo->tm_mon + 1;
    int day   = timeinfo->tm_mday;

    sunriseHour = solarCalc.calcSunrise(year, month, day);
    sunsetHour  = solarCalc.calcSunset(year, month, day);

    Serial.printf("☀️ Lever: %.2f | Coucher: %.2f\n", sunriseHour, sunsetHour);
}

// =============================================

void handleTimeUtils() {
    unsigned long currentMillis = millis();

    // Mise à jour NTP toutes les 10 secondes
    if (currentMillis - lastTimeUpdate > 10000) {
        timeClient.update();
        lastTimeUpdate = currentMillis;
    }

    // Recalcul du lever/coucher une fois par jour (à minuit)
    if (currentMillis - lastSunCalc > 3600000UL) {  // toutes les heures pour sécurité
        struct tm *timeinfo = localtime(&timeClient.getEpochTime());
        if (timeinfo->tm_hour == 0 && (currentMillis - lastSunCalc > 3600000UL)) {
            calculateSunTimes();
            lastSunCalc = currentMillis;
        }
    }

    // Mise à jour de l'état global isNight
    isNight = isCurrentlyNight();
}

// =============================================

bool isCurrentlyNight() {
    if (!timeClient.isTimeSet()) return true;  // Par sécurité : on considère nuit si pas d'heure

    time_t now = timeClient.getEpochTime();
    struct tm *timeinfo = localtime(&now);

    double currentHour = timeinfo->tm_hour + (timeinfo->tm_min / 60.0);

    // Ajoute une petite marge (ex: 30 min après coucher, 30 min avant lever)
    return (currentHour < (sunriseHour - 0.5)) || (currentHour > (sunsetHour + 0.5));
}

// =============================================

String getCurrentTimeFormatted() {
    time_t now = timeClient.getEpochTime();
    struct tm *timeinfo = localtime(&now);
    
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    return String(buffer);
}