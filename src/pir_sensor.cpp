#include "pir_sensor.h"
#include "config.h"
#include "relay_control.h"

unsigned long lastMotionTime = 0;
const unsigned long motionTimeout = 300000; // 5 minutes

void setupPIR() {
    pinMode(PIR_PIN, INPUT);
    Serial.println("Capteur PIR initialisé");
}

void handlePIR() {
    if (isMotionDetected()) {
        lastMotionTime = millis();
        
        if (isNight || manualOverride) {
            turnRelayOn();
            Serial.println("🔦 Mouvement détecté → Relais ON");
        }
    }
}

bool isMotionDetected() {
    return digitalRead(PIR_PIN) == HIGH;
}