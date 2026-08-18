#include "Astro.h"
#include "Storage.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>
#include <time.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

bool isDayTime = true; // Par défaut à true pour éviter l'affichage de nuit au boot

float latitude = 0.0;
float longitude = 0.0;
int utcOffset = 3600;
int lastCheckedDay = -1;
int cachedSunrise = 0;
int cachedSunset = 0;

bool locationLoaded = false;
unsigned long lastLocationRetry = 0;
const unsigned long LOCATION_RETRY_INTERVAL = 300000; // 5 minutes

// --- CALCUL INTERNE DU SOLEIL (NOAA corrigé et robuste) ---
float toRad(float deg) { return deg * PI / 180.0; }
float toDeg(float rad) { return rad * 180.0 / PI; }

int calculateSunrise(int year, int month, int day) {
    int N1 = 275 * month / 9;
    int N2 = (month + 9) / 12;
    int N3 = (1 + (year - 4 * (year / 4) + 3) / 4);
    int N = N1 - (N2 * N3) + day - 30;

    float longitudeHour = longitude / 15.0;
    float tRise = N + ((6.0 - longitudeHour) / 24.0);

    float M = (0.9856 * tRise) - 3.289;
    float L = M + (1.916 * sin(toRad(M))) + (0.020 * sin(toRad(2 * M))) + 282.634;
    while (L >= 360.0) L -= 360.0;
    while (L < 0.0) L += 360.0;

    float RA = toDeg(atan(0.91746 * tan(toRad(L))));
    while (RA >= 360.0) RA -= 360.0;
    while (RA < 0.0) RA += 360.0;

    float Lquadrant  = (floor(L / 90.0)) * 90.0;
    float RAquadrant = (floor(RA / 90.0)) * 90.0;
    RA = RA + (Lquadrant - RAquadrant);
    RA = RA / 15.0;

    float sinDec = 0.39782 * sin(toRad(L));
    float cosDec = cos(asin(sinDec));

    float latRad = toRad(latitude);
    float cosH = (cos(toRad(90.833)) - (sin(latRad) * sinDec)) / (cos(latRad) * cosDec);
    
    if (cosH > 1.0 || cosH < -1.0) return -1;

    float H = 360.0 - toDeg(acos(cosH));
    H = H / 15.0;

    float T = H + RA - (0.06571 * tRise) - 6.622;
    float UT = T - longitudeHour;
    
    while (UT < 0.0) UT += 24.0;
    while (UT >= 24.0) UT -= 24.0;
    
    float localTime = UT + ((float)utcOffset / 3600.0);
    while (localTime < 0.0) localTime += 24.0;
    while (localTime >= 24.0) localTime -= 24.0;

    return (int)(localTime * 60 + 0.5);
}

int calculateSunset(int year, int month, int day) {
    int N1 = 275 * month / 9;
    int N2 = (month + 9) / 12;
    int N3 = (1 + (year - 4 * (year / 4) + 3) / 4);
    int N = N1 - (N2 * N3) + day - 30;

    float longitudeHour = longitude / 15.0;
    float tSet = N + ((18.0 - longitudeHour) / 24.0);

    float M = (0.9856 * tSet) - 3.289;
    float L = M + (1.916 * sin(toRad(M))) + (0.020 * sin(toRad(2 * M))) + 282.634;
    while (L >= 360.0) L -= 360.0;
    while (L < 0.0) L += 360.0;

    float RA = toDeg(atan(0.91746 * tan(toRad(L))));
    while (RA >= 360.0) RA -= 360.0;
    while (RA < 0.0) RA += 360.0;

    float Lquadrant  = (floor(L / 90.0)) * 90.0;
    float RAquadrant = (floor(RA / 90.0)) * 90.0;
    RA = RA + (Lquadrant - RAquadrant);
    RA = RA / 15.0;

    float sinDec = 0.39782 * sin(toRad(L));
    float cosDec = cos(asin(sinDec));

    float latRad = toRad(latitude);
    float cosH = (cos(toRad(90.833)) - (sin(latRad) * sinDec)) / (cos(latRad) * cosDec);
    
    if (cosH > 1.0 || cosH < -1.0) return -1;

    float H = toDeg(acos(cosH));
    H = H / 15.0;

    float T = H + RA - (0.06571 * tSet) - 6.622;
    float UT = T - longitudeHour;
    
    while (UT < 0.0) UT += 24.0;
    while (UT >= 24.0) UT -= 24.0;
    
    float localTime = UT + ((float)utcOffset / 3600.0);
    while (localTime < 0.0) localTime += 24.0;
    while (localTime >= 24.0) localTime -= 24.0;

    return (int)(localTime * 60 + 0.5);
}
// ------------------------------------------------

bool isDST(int day, int month, int dow) {
    if (month < 3 || month > 10) return false;
    if (month > 3 && month < 10) return true;
    int previousSunday = day - dow;
    if (month == 3) return previousSunday >= 25;
    if (month == 10) return previousSunday < 25;
    return false;
}

// Fonction HTTP isolée pour récupérer la position via l'IP
bool fetchLocationFromIP() {
    HTTPClient http;
    WiFiClient client;
    bool success = false;

    http.begin(client, "http://api.ipify.org");
    if (http.GET() == 200) {
        String ip = http.getString();
        Serial.print("IP Publique: "); Serial.println(ip);
        
        http.end();
        http.begin(client, "http://ip-api.com/json/" + ip + "?fields=status,lat,lon,offset");
        if (http.GET() == 200) {
            String payload = http.getString();
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            
            if (doc["status"] == "success") {
                latitude = doc["lat"].as<float>();
                longitude = doc["lon"].as<float>();
                utcOffset = doc["offset"].as<int>(); 
                saveLocation(latitude, longitude, utcOffset);
                success = true;
            }
        }
    }
    http.end();
    return success;
}

void initAstro() {
    if (loadLocation(latitude, longitude, utcOffset)) {
        Serial.println("Localisation trouvée en mémoire.");
        locationLoaded = true;
    } else {
        Serial.println("Aucune localisation en mémoire. Essai via IP...");
        if (fetchLocationFromIP()) {
            locationLoaded = true;
            Serial.println("Localisation récupérée et sauvegardée !");
        } else {
            Serial.println("Échec de la récupération IP au démarrage (nouvel essai en arrière-plan prévu).");
            locationLoaded = false;
        }
    }

    configTime(utcOffset, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("Horloge NTP synchronisée.");
}

void updateAstroState() {
    if (!locationLoaded) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastLocationRetry >= LOCATION_RETRY_INTERVAL || lastLocationRetry == 0) {
            lastLocationRetry = currentMillis;
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("[ASTRO] Nouvelle tentative de récupération de position en arrière-plan...");
                if (fetchLocationFromIP()) {
                    locationLoaded = true;
                    configTime(utcOffset, 0, "pool.ntp.org", "time.nist.gov");
                    Serial.println("[ASTRO] Position trouvée et sauvegardée ! Fin des essais.");
                }
            }
        }
        return; 
    }

    if (latitude == 0.0 && longitude == 0.0) return;

    time_t now = time(nullptr);
    struct tm timeinfo_utc;
    gmtime_r(&now, &timeinfo_utc);

    if (timeinfo_utc.tm_year + 1900 < 2024) {
        static unsigned long lastNtpWarn = 0;
        if (millis() - lastNtpWarn > 5000) {
            Serial.println("[ASTRO] Attente heure NTP...");
            lastNtpWarn = millis();
        }
        return; 
    }
    
    int year = timeinfo_utc.tm_year + 1900;
    int month = timeinfo_utc.tm_mon + 1;
    int day = timeinfo_utc.tm_mday;
    int currentDay = timeinfo_utc.tm_yday;

    if (currentDay != lastCheckedDay) {
        lastCheckedDay = currentDay;
        cachedSunrise = calculateSunrise(year, month, day);
        cachedSunset  = calculateSunset(year, month, day);
        
        if (cachedSunrise >= 0 && cachedSunset >= 0) {
             Serial.println("=============================================");
             Serial.printf("[ASTRO] CALCUL POUR %04d-%02d-%02d\n", year, month, day);
             Serial.printf("[ASTRO] Lever  : %02d:%02d\n", cachedSunrise / 60, cachedSunrise % 60);
             Serial.printf("[ASTRO] Coucher : %02d:%02d\n", cachedSunset / 60, cachedSunset % 60);
             Serial.println("=============================================");
        }
    }

    if (cachedSunrise < 0 || cachedSunset < 0) {
        isDayTime = true; 
        return;
    }

    struct tm timeinfo_local;
    localtime_r(&now, &timeinfo_local);
    int currentMinutes = timeinfo_local.tm_hour * 60 + timeinfo_local.tm_min;

    bool newState = (currentMinutes >= cachedSunrise && currentMinutes < cachedSunset);

    if (newState != isDayTime) {
        isDayTime = newState;
        Serial.printf("[ASTRO] ===> BASCULEMENT : C'est desormais le %s (Il est %02d:%02d) <===\n", 
                      isDayTime ? "JOUR" : "NUIT", 
                      timeinfo_local.tm_hour, timeinfo_local.tm_min);
    }
}