#include "Storage.h"
#include <LittleFS.h>

void initFileSystem() {
    if (!LittleFS.begin()) {
        Serial.println("Erreur critique: LittleFS impossible à monter !");
        return;
    }
    Serial.println("=== Contenu de LittleFS ===");
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
        Serial.print(" - Fichier: ");
        Serial.println(dir.fileName());
    }
    Serial.println("============================");
}

String loadHTMLFile(String path) {
    File file = LittleFS.open(path, "r");
    if (!file) return "Fichier non trouve: " + path;
    String content = file.readString();
    file.close();
    return content;
}

void saveCredentials(String ssid, String pass) {
    File file = LittleFS.open("/wifi.txt", "w");
    if (!file) return;
    file.println(ssid);
    file.println(pass);
    file.close();
    Serial.println("Identifiants sauvegardés.");
}

bool loadCredentials(String &ssid, String &pass) {
    if (!LittleFS.exists("/wifi.txt")) return false;
    File file = LittleFS.open("/wifi.txt", "r");
    if (!file) return false;
    ssid = file.readStringUntil('\n');
    pass = file.readStringUntil('\n');
    ssid.trim(); pass.trim();
    file.close();
    return (ssid.length() > 0);
}

void deleteCredentials() { 
    LittleFS.remove("/wifi.txt"); 
}

void saveLocation(float lat, float lon, int offset) {
    File file = LittleFS.open("/location.txt", "w");
    if (!file) return;
    file.println(lat, 6);
    file.println(lon, 6);
    file.println(offset);
    file.close();
    Serial.println("Localisation sauvegardée.");
}

bool loadLocation(float &lat, float &lon, int &offset) {
    if (!LittleFS.exists("/location.txt")) return false;
    File file = LittleFS.open("/location.txt", "r");
    if (!file) return false;
    lat = file.readStringUntil('\n').toFloat();
    lon = file.readStringUntil('\n').toFloat();
    offset = file.readStringUntil('\n').toInt();
    file.close();
    return (lat != 0.0 || lon != 0.0);
}

void deleteLocation() { 
    LittleFS.remove("/location.txt"); 
}

void saveDayNightState(bool enabled) {
    File file = LittleFS.open("/daynight.txt", "w");
    if (!file) return;
    file.println(enabled ? "1" : "0");
    file.close();
    Serial.println("État Jour/Nuit sauvegardé.");
}

bool loadDayNightState(bool &enabled) {
    if (!LittleFS.exists("/daynight.txt")) {
        enabled = true; // Valeur par défaut à true (ON) si le fichier n'existe pas
        return false;
    }
    File file = LittleFS.open("/daynight.txt", "r");
    if (!file) {
        enabled = true;
        return false;
    }
    String val = file.readStringUntil('\n');
    val.trim();
    file.close();
    
    enabled = (val == "1");
    return true;
}