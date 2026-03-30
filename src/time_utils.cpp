#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SolarCalculator.h>
#include <time.h>
#include <TZ.h>

SolarCalculator solarCalc;
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org");

void extractCityInfo(const char* cityInfo, double& latitude, double& longitude, char* timezone) {
    char* token;
    char buffer[80];
    strcpy(buffer, cityInfo);

    // Extraire la latitude
    token = strtok(buffer, "|");
    token = strtok(NULL, "|");
    latitude = atof(token);

    // Extraire la longitude
    token = strtok(NULL, "|");
    longitude = atof(token);

    // Extraire le fuseau horaire
    token = strtok(NULL, "|");
    strcpy(timezone, token);
}

void setupTimeUtils(const char* cityInfo) {
    double latitude, longitude;
    char timezone[40];

    extractCityInfo(cityInfo, latitude, longitude, timezone);

    timeClient.begin();
    solarCalc.setPosition(latitude, longitude);
    configTime(timezone, "pool.ntp.org");
}

bool isNight() {
    timeClient.update();
    time_t now = timeClient.getEpochTime();
    struct tm *timeinfo = localtime(&now);

    double sunrise = solarCalc.calcSunrise(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    double sunset = solarCalc.calcSunset(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);

    double currentHour = timeinfo->tm_hour + (timeinfo->tm_min / 60.0);
    return currentHour < sunrise || currentHour > sunset;
}