#include "ota_handler.h"

void setupOTA() {
  ArduinoOTA.setHostname("Projecteur-X2");
  // ArduinoOTA.setPassword("monmotdepasse");  // Désactivé pour les tests
  ArduinoOTA.onStart([]() { Serial.println("Début OTA..."); });
  ArduinoOTA.onEnd([]() { Serial.println("OTA terminé !"); });
  ArduinoOTA.onError([](ota_error_t error) { Serial.printf("Erreur OTA: %u\n", error); });
  ArduinoOTA.begin();
}

void handleOTA() {
  ArduinoOTA.handle();
}