#include "relay_control.h"
#include "config.h"

bool relayState = false;

void setupRelay() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);  // Relais OFF au démarrage
    Serial.println("Relais initialisé (OFF)");
}

void turnRelayOn() {
    digitalWrite(RELAY_PIN, HIGH);
    relayState = true;
}

void turnRelayOff() {
    digitalWrite(RELAY_PIN, LOW);
    relayState = false;
}

bool isRelayOn() {
    return relayState;
}

void handleRelay() {
    // Mode manuel prioritaire
    if (manualOverride) {
        return; // On laisse l'utilisateur contrôler manuellement
    }

    // Logique automatique : seulement la nuit + mouvement
    if (isNight && isMotionDetected()) {
        turnRelayOn();
    } 
    else if (!isNight) {
        turnRelayOff();
    }
}