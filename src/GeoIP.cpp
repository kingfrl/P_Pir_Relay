/**
 * =============================================
 * GeoIP.cpp - Récupération automatique de localisation
 * =============================================
 */

#include <ESP8266WiFi.h>
 #include "GeoIP.h"
#include "config.h"
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define EEPROM_SIZE          512
#define LOCATION_EEPROM_ADDR 0

// =============================================

void fetchLocationData() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ WiFi non connecté, impossible de récupérer la localisation");
        return;
    }

    Serial.println("🌍 Récupération des données de localisation...");

    HTTPClient http;
    http.setTimeout(8000);
    http.begin("http://ip-api.com/json/");

    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(1024);
        
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error && strcmp(doc["status"], "success") == 0) {
            const char* city = doc["city"] | "Inconnue";
            double lat = doc["lat"] | 0.0;
            double lon = doc["lon"] | 0.0;
            const char* tz  = doc["timezone"] | "Europe/Paris";

            dtostrf(lat, 6, 4, latitudeValue);
            dtostrf(lon, 6, 4, longitudeValue);
            strncpy(cityValue, city, 39);
            strncpy(timezoneValue, tz, 39);
            
            cityValue[39] = '\0';
            timezoneValue[39] = '\0';

            Serial.printf("✅ Localisation OK → %s (%.4f, %.4f) | TZ: %s\n", 
                         cityValue, lat, lon, timezoneValue);

            saveLocationToEEPROM();        // Sauvegarde persistante
        } 
        else {
            Serial.println("⚠️ Réponse API invalide");
        }
    } 
    else {
        Serial.printf("❌ Erreur HTTP: %d\n", httpCode);
    }

    http.end();
}

// =============================================
// Sauvegarde dans l'EEPROM (persistance après redémarrage)
// =============================================

void saveLocationToEEPROM() {
    EEPROM.begin(EEPROM_SIZE);
    
    struct LocationData {
        char city[40];
        char lat[12];
        char lon[12];
        char tz[40];
    } data;

    strncpy(data.city, cityValue, 39);
    strncpy(data.lat,  latitudeValue, 11);
    strncpy(data.lon,  longitudeValue, 11);
    strncpy(data.tz,   timezoneValue, 39);

    EEPROM.put(LOCATION_EEPROM_ADDR, data);
    EEPROM.commit();
    EEPROM.end();

    Serial.println("💾 Localisation sauvegardée en EEPROM");
}

// =============================================

void loadLocationFromEEPROM() {
    EEPROM.begin(EEPROM_SIZE);
    
    struct LocationData {
        char city[40];
        char lat[12];
        char lon[12];
        char tz[40];
    } data;

    EEPROM.get(LOCATION_EEPROM_ADDR, data);
    EEPROM.end();

    // Si les données semblent valides
    if (strlen(data.lat) > 3 && atof(data.lat) != 0.0) {
        strncpy(cityValue, data.city, 39);
        strncpy(latitudeValue, data.lat, 11);
        strncpy(longitudeValue, data.lon, 11);
        strncpy(timezoneValue, data.tz, 39);

        Serial.printf("📖 Localisation chargée depuis EEPROM : %s\n", cityValue);
    } else {
        Serial.println("⚠️ Aucune donnée valide en EEPROM (utilisation des valeurs par défaut)");
    }
}

// =============================================

bool isLocationValid() {
    return (atof(latitudeValue) != 0.0 && atof(longitudeValue) != 0.0);
}