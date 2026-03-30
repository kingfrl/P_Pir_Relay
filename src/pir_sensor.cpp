#include "pir_sensor.h"
#include <Arduino.h>

const int PIN_PIR = 4;
unsigned long lastMotionTime = 0;

void setupPIR() {
  pinMode(PIN_PIR, INPUT);
}

bool isMotionDetected() {
  return digitalRead(PIN_PIR) == HIGH;
}

void handlePIR() {
  if (isMotionDetected()) {
    lastMotionTime = millis();
    // Logique de détection ici (ex: allumer le relais si conditions remplies)
  }
}