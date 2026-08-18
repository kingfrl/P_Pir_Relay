#include "Network.h"
#include "Storage.h"
#include "Config.h"
#include "Astro.h"
#include "Hardware.h"
#include <ArduinoJson.h>

DNSServer dnsServer;
ESP8266WebServer server(80);

State currentState = IDLE;
bool isAPModeActive = false;
bool isServerRunning = false;
String networkListHTML = "";
String deviceName = "interi_" + String(ESP.getChipId(), HEX);
String apSSID = ""; 

String selectedSSID = "";
String selectedPASS = "";
unsigned long connectingStartTime = 0;

void scanNetworks() {
    networkListHTML = "";
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        int q = WiFi.RSSI(i);
        String s = (q <= -80) ? "Faible" : (q <= -60) ? "Moyen" : "Fort";
        networkListHTML += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + " (" + s + ")</option>";
    }
}

void startSilentConnection(String ssid, String pass) {
    selectedSSID = ssid;
    selectedPASS = pass;
    WiFi.begin(selectedSSID.c_str(), selectedPASS.c_str());
    currentState = CONNECTING;
    connectingStartTime = millis();
}

void handleRoot() {
    String html;
    if (currentState == CONNECTED) {
        String clientIP = server.client().localIP().toString();
        html = clientIP.startsWith("192.168.4.") ? loadHTMLFile("/success.html") : loadHTMLFile("/index.html");
        html.replace("%DEVICENAME%", deviceName);
        html.replace("%IP%", WiFi.localIP().toString());
    } else if (currentState == CONNECTING) {
        html = loadHTMLFile("/wait.html");
    } else {
        html = loadHTMLFile("/setup.html");
        html.replace("%LIST%", networkListHTML);
        if (currentState == FAILED) {
            html.replace("%MSG%", "Echec de la connexion. Vérifiez le MDP.");
            html.replace("%DISPLAY_MSG%", "block");
        } else {
            html.replace("%MSG%", "");
            html.replace("%DISPLAY_MSG%", "none");
        }
    }
    server.send(200, "text/html; charset=utf-8", html);
}

void handleConnect() {
    if (server.hasArg("ssid") && server.hasArg("pass")) {
        startSilentConnection(server.arg("ssid"), server.arg("pass"));
        server.sendHeader("Location", "/");
        server.send(303);
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleReboot() {
    server.send(200, "text/html; charset=utf-8", "<html><body><h2>Reboot...</h2></body></html>");
    delay(1000); ESP.restart();
}

void handleReset() {
    deleteCredentials();
    deleteLocation();
    server.send(200, "text/html; charset=utf-8", "<html><body><h2>Effacé. Reboot...</h2></body></html>");
    delay(1000); ESP.restart();
}

// ==========================================
// API JSON (Pour la page Web)
// ==========================================
void handleApiStatus() {
    DynamicJsonDocument doc(256);
    doc["isDayTime"] = isDayTime;
    doc["dayNightEnabled"] = dayNightEnabled;
    doc["pirState"] = pirState;
    doc["relayState"] = relayState;
    doc["remainingSeconds"] = remainingSeconds;
    
    if (currentMode == MANUAL) doc["currentMode"] = "MANUAL";
    else if (currentMode == AUTO) doc["currentMode"] = "AUTO";
    else doc["currentMode"] = "ALARM";

    String jsonStr;
    serializeJson(doc, jsonStr);
    server.send(200, "application/json; charset=utf-8", jsonStr);
}

void handleApiAction() {
    if (server.hasArg("plain") == false) {
        server.send(400, "application/json", "{\"error\":\"Corps vide\"}");
        return;
    }
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error) {
        server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
        return;
    }

    String action = doc["action"].as<String>();
    String strValue = doc["value"].as<String>();
    int intValue = strValue.toInt();

    handleHardwareAction(action, strValue, intValue);
    server.send(200, "application/json; charset=utf-8", "{\"status\":\"ok\"}");
}

// ==========================================
// API REST (Pour scripts externes / requêtes simples)
// ==========================================
void handleUrlStatus() {
    String reponse = "Relais:" + String(relayState ? "ON" : "OFF") + 
                     "|PIR:" + String(pirState ? "OUI" : "NON") + 
                     "|Mode:" + String((currentMode == MANUAL) ? "MANUEL" : (currentMode == AUTO) ? "AUTO" : "ALARME") + 
                     "|Jour:" + String(isDayTime ? "OUI" : "NON") + 
                     "|ContrainteJN:" + String(dayNightEnabled ? "OUI" : "NON");
    server.send(200, "text/plain", reponse);
}

void handleUrlRelaisOn() {
    if (!relayState) handleHardwareAction("TOGGLE_RELAY", "", 0); 
    server.send(200, "text/plain", "OK");
}

void handleUrlRelaisOff() {
    if (relayState) handleHardwareAction("TOGGLE_RELAY", "", 0); 
    server.send(200, "text/plain", "OK");
}

void handleUrlSetMode() {
    // Extrait la fin de l'URL (ex: "/mode/auto" devient "auto")
    String path = server.uri();
    String requestedMode = path.substring(path.lastIndexOf('/') + 1);
    requestedMode.toUpperCase(); // MANUAL, AUTO, ALARM
    
    if (requestedMode == "MANUAL" || requestedMode == "AUTO" || requestedMode == "ALARM") {
        handleHardwareAction("SET_MODE", requestedMode, 0);
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Mode inconnu");
    }
}

void handleUrlJourOn() {
    if (!dayNightEnabled) handleHardwareAction("TOGGLE_DAYNIGHT", "", 0);
    server.send(200, "text/plain", "OK");
}

void handleUrlJourOff() {
    if (dayNightEnabled) handleHardwareAction("TOGGLE_DAYNIGHT", "", 0);
    server.send(200, "text/plain", "OK");
}

void handleUrlTimerManual() {
    if (server.hasArg("v")) {
        handleHardwareAction("SET_MANUAL_TIMER", "", server.arg("v").toInt());
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Erreur: Ajoutez ?v=Nombre");
    }
}

void handleUrlTimerAuto() {
    if (server.hasArg("v")) {
        handleHardwareAction("SET_AUTO_TIMER", "", server.arg("v").toInt());
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Erreur: Ajoutez ?v=Nombre");
    }
}

// ==========================================
// Démarrage des serveurs
// ==========================================
void startAPMode() {
    WiFi.mode(WIFI_AP_STA);
    apSSID = deviceName; 
    WiFi.softAP(apSSID.c_str()); 
    dnsServer.start(53, "*", WiFi.softAPIP());
    
    server.on("/generate_204", []() { server.sendHeader("Location", "http://192.168.4.1/"); server.send(302, "text/plain", ""); });
    server.on("/hotspot-detect.html", []() { server.sendHeader("Location", "http://192.168.4.1/"); server.send(302, "text/plain", ""); });
    server.on("/redirect", []() { server.sendHeader("Location", "http://192.168.4.1/"); server.send(302, "text/plain", ""); });
    
    scanNetworks();
    server.on("/", handleRoot);
    server.on("/connect", HTTP_POST, handleConnect);
    server.on("/reboot", HTTP_GET, handleReboot);
    server.on("/reset", HTTP_GET, handleReset);
    
    // API JSON
    server.on("/api/status", HTTP_GET, handleApiStatus);
    server.on("/api/action", HTTP_POST, handleApiAction);
    
    // API REST
    server.on("/status", HTTP_GET, handleUrlStatus);
    server.on("/relais/on", HTTP_GET, handleUrlRelaisOn);
    server.on("/relais/off", HTTP_GET, handleUrlRelaisOff);
    server.on("/mode/manual", HTTP_GET, handleUrlSetMode);
    server.on("/mode/auto", HTTP_GET, handleUrlSetMode);
    server.on("/mode/alarm", HTTP_GET, handleUrlSetMode);
    server.on("/jour/on", HTTP_GET, handleUrlJourOn);
    server.on("/jour/off", HTTP_GET, handleUrlJourOff);
    server.on("/timer/manuel", HTTP_GET, handleUrlTimerManual);
    server.on("/timer/auto", HTTP_GET, handleUrlTimerAuto);

    server.begin();
    isAPModeActive = true;
    isServerRunning = true;
    currentState = IDLE;
    Serial.println("Mode AP actif.");
}

void startNormalMode() {
    server.on("/", handleRoot);
    server.on("/reset", HTTP_GET, handleReset);
    
    // API JSON
    server.on("/api/status", HTTP_GET, handleApiStatus);
    server.on("/api/action", HTTP_POST, handleApiAction);
    
    // API REST
    server.on("/status", HTTP_GET, handleUrlStatus);
    server.on("/relais/on", HTTP_GET, handleUrlRelaisOn);
    server.on("/relais/off", HTTP_GET, handleUrlRelaisOff);
    server.on("/mode/manual", HTTP_GET, handleUrlSetMode);
    server.on("/mode/auto", HTTP_GET, handleUrlSetMode);
    server.on("/mode/alarm", HTTP_GET, handleUrlSetMode);
    server.on("/jour/on", HTTP_GET, handleUrlJourOn);
    server.on("/jour/off", HTTP_GET, handleUrlJourOff);
    server.on("/timer/manuel", HTTP_GET, handleUrlTimerManual);
    server.on("/timer/auto", HTTP_GET, handleUrlTimerAuto);

    server.begin();
    isServerRunning = true;
    Serial.println("Mode Normal actif (API REST OK).");
}

void loopNetwork() {
    if (isAPModeActive) dnsServer.processNextRequest();
    if (isServerRunning) server.handleClient();

    if (currentState == CONNECTING) {
        if (WiFi.status() == WL_CONNECTED) {
            currentState = CONNECTED;
            Serial.print("\nConnecté ! IP: "); Serial.println(WiFi.localIP());
            if (isAPModeActive) {
                saveCredentials(selectedSSID, selectedPASS);
            } else {
                startNormalMode();
            }
        } else if (millis() - connectingStartTime > CONNECTION_TIMEOUT) {
            Serial.println("Timeout connexion.");
            WiFi.disconnect();
            currentState = FAILED;
            if (!isAPModeActive) startAPMode();
        }
    }
}