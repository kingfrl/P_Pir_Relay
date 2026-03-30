#include "relay_control.h"
#include "pir_sensor.h"
#include "mqtt_client.h"
#include "time_utils.h"

// Variables globales pour les durées
unsigned long flashDuration = 600;  // 10 minutes
unsigned long manualDuration = 14400;  // 4 heures
unsigned long autoDuration = 30;  // 30 secondes

const int PIN_RELAY = 5;
unsigned long relayTimer = 0;
bool relayActive = false;

// Variables pour suivre les modes actifs
bool flashModeActive = false;
bool partyModeActive = false;

void setupRelay() {
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);
}

void setRelay(bool state) {
  digitalWrite(PIN_RELAY, state ? HIGH : LOW);
  relayActive = state;
  if (state) relayTimer = millis();
  publishRelayState(state);
}

bool getRelayState() {
  return relayActive;
}

void handleRelay() {
  if (relayActive && (millis() - relayTimer > getActiveDuration() * 1000)) {
    setRelay(false);
  }
}

unsigned long getActiveDuration() {
  if (partyModeActive) return manualDuration;
  if (flashModeActive) return flashDuration;
  return autoDuration;
}

// Fonctions pour activer/désactiver les modes
void setFlashMode(bool active) {
  flashModeActive = active;
}

void setPartyMode(bool active) {
  partyModeActive = active;
}