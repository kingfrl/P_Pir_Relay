#include "ota_handler.h"
#include <Arduino.h>

void setupOTA() {
    Serial.println("OTA : ElegantOTA désactivé temporairement (incompatible simple avec IotWebConf v2)");
    Serial.println("     Utilise l'upload du firmware via la page /config pour le moment");
}

void handleOTA() {
    // Vide pour l'instant
}