#include "web_interface.h"
#include "config.h"
#include "time_utils.h"
#include <IotWebConf.h>

extern IotWebConf iotWebConf;

void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<title>" PROJECT_NAME "</title>";
    html += "<style>body{font-family:Arial;text-align:center;margin-top:50px;}</style></head><body>";
    html += "<h1>" PROJECT_NAME " v" PROJECT_VERSION "</h1>";
    html += "<p><strong>IP :</strong> " + WiFi.localIP().toString() + "</p>";
    html += "<p><strong>Ville :</strong> " + String(cityValue) + "</p>";
    html += "<p><strong>Lever :</strong> " + String(sunriseHour, 2) + "h</p>";
    html += "<p><strong>Coucher :</strong> " + String(sunsetHour, 2) + "h</p>";
    html += "<p><strong>État :</strong> " + String(isNight ? "🌙 NUIT" : "☀️ JOUR") + "</p>";
    html += "<hr>";
    html += "<p><a href='/config'>⚙️ Configuration</a></p>";
    html += "</body></html>";

    iotWebConf.getHtmlFormatProvider()->sendPage(html);
}

void setupWebInterface() {
    iotWebConf.setCustomHtmlRootHandler(handleRoot);
}

void handleWebInterface() {
    // Rien à faire
}