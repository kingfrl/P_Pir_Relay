#include "web_interface.h"
#include "wifi_manager.h"
#include "relay_control.h"

ESP8266WebServer server(80);

void setupWebInterface() {
  server.on("/", []() {
    String html = "<html><body>";
    html += "<h1>Contrôle du Projecteur</h1>";
    html += "<button onclick='fetch(\"/relay/on\")'>Allumer</button>";
    html += "<button onclick='fetch(\"/relay/off\")'>Éteindre</button>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/relay/on", []() {
    setRelay(true);
    server.send(200, "text/plain", "Relais ON");
  });

  server.on("/relay/off", []() {
    setRelay(false);
    server.send(200, "text/plain", "Relais OFF");
  });

  server.begin();
}

void handleWebInterface() {
  server.handleClient();
}