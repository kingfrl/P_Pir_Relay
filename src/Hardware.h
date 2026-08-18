#pragma once
#include <Arduino.h>

// Les 3 modes de fonctionnement
enum Mode { MANUAL, AUTO, ALARM };

// Variables d'état accessibles par le serveur Web
extern Mode currentMode;
extern bool pirState;
extern bool relayState;
extern int remainingSeconds;
extern int manualTimerMinutes;
extern int autoTimerMinutes;
extern bool dayNightEnabled;

// Fonctions
void initHardware();
void updateHardware();
void handleHardwareAction(String action, String strValue, int intValue);