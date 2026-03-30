#include "web_interface.h"
#include "config.h"
#include "time_utils.h"
#include <IotWebConf.h>
extern IotWebConf iotWebConf;

void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<title>" PROJECT_NAME "</title>";
    html += "<style>body{font-family:Arial; text-align:center;}</style></head><body>";
    html += "<h1>" PROJECT_NAME " v" PROJECT_VERSION "</h1>";
    html += "<p><strong>Statut :</strong> Connecté</p>";
    html += "<p><strong>IP :</strong> " + WiFi.localIP().toString() + "</p>";
    html += "<p><strong>Ville :</strong> " + String(cityValue) + "</p>";
    html += "<p><strong>Lever du soleil :</strong> " + String(sunriseHour, 2) + "h</p>";
    html += "<p><strong>Coucher du soleil :</strong> " + String(sunsetHour, 2) + "h</p>";
    html += "<p><strong>Mode actuel :</strong> " + String(isNight ? "🌙 NUIT" : "☀️ JOUR") + "</p>";
    html += "<hr>";
    html += "<p><a href='/config'>⚙️ Configuration</a> | ";
    html += "<a href='/ota'>📤 Mise à jour OTA</a></p>";
    html += "</body></html>";

    iotWebConf.getWebServer()->send(200, "text/html", html);
}

void setupWebInterface() {
    iotWebConf.getWebServer()->on("/", handleRoot);
}

void handleWebInterface() {
    // Rien à faire ici, IotWebConf gère tout
}