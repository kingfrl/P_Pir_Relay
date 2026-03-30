#include "wifi_manager.h"
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <IotWebConf.h>

extern ESP8266WebServer server;
extern DNSServer dnsServer;

struct RTC_Data {
  uint32_t magic = 0xAA55AA55;
  uint32_t resetCount = 0;
  unsigned long lastResetTime;
} rtcData;

void setupWiFi() {
  ESP.rtcUserMemoryRead(0, (uint32_t*)&rtcData, sizeof(rtcData));
  if (rtcData.magic != 0xAA55AA55 || WiFi.SSID() == "") {
    startAPMode();  // Démarre en mode AP si première utilisation ou double appui RESET
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin();
    connectToWiFi(WiFi.SSID().c_str(), WiFi.psk().c_str());
  }
}

void handleWiFi() {
  // Gestion continue du Wi-Fi (reconnexion, etc.)
}

void startAPMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP-Config", "12345678");
  Serial.println("Mode AP activé. IP: 192.168.4.1");
}

void connectToWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnecté ! IP: " + WiFi.localIP().toString());
  } else {
    startAPMode();  // Retour en mode AP si échec
  }
}