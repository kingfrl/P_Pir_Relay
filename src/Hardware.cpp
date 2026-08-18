#include "Hardware.h"
#include "Astro.h"

// Configuration des broches
#define RELAY_PIN 5
#define PIR_PIN 4

// Norme pour relais "Active Low" (Très courant sur les cartes 220V avec optocoupleur)
// Si ton relais s'allume quand tu mets HIGH, inverse ces deux valeurs.
#define RELAY_ON LOW
#define RELAY_OFF HIGH

// Variables d'état (accessibles via le .h)
Mode currentMode = MANUAL;
bool pirState = false;
bool relayState = false;
int remainingSeconds = 0;
int manualTimerMinutes = 0;
int autoTimerMinutes = 5;
bool dayNightEnabled = false;

// Variables internes de gestion
unsigned long timerStartTime = 0;
bool waitForPirLow = false;

// --- Fonctions internes ---

void setRelay(bool state) {
    relayState = state;
    digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);
}

void startTimer(int minutes) {
    if (minutes > 0) {
        timerStartTime = millis();
        remainingSeconds = minutes * 60;
    } else {
        timerStartTime = 0;
        remainingSeconds = 0;
    }
}

// --- Fonctions publiques ---

void initHardware() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, RELAY_OFF); // Toujours éteint au démarrage par sécurité
    pinMode(PIR_PIN, INPUT);
    
    Serial.println("Module Hardware initialisé (Relais=D2, PIR=D5)");
}

void updateHardware() {
    // 1. Lecture du capteur PIR
    bool currentPir = (digitalRead(PIR_PIN) == HIGH);
    
    // Gestion du drapeau anti-rebond logique
    if (waitForPirLow) {
        if (!currentPir) {
            waitForPirLow = false; // Le PIR est retombé, on réarme la détection
        }
        // Tant qu'on attend que le PIR passe à 0, on ignore complètement son état
        pirState = false; 
    } else {
        pirState = currentPir;
    }

    // 2. Mise à jour de la minuterie (si elle est active)
    if (timerStartTime > 0) {
        unsigned long elapsed = (millis() - timerStartTime) / 1000;
        if (elapsed >= 1) { // On met à jour toutes les secondes
            if (elapsed >= (unsigned long)remainingSeconds) {
                // Le temps est écoulé
                setRelay(false);
                timerStartTime = 0;
                remainingSeconds = 0;
            } else {
                remainingSeconds -= elapsed;
                timerStartTime = millis(); // Réajuste la base de temps pour la seconde suivante
            }
        }
    }

    // 3. Logique d'automatisation en fonction du mode
    if (currentMode == ALARM) {
        // En mode alarme, le relais doit être FORCÉMENT éteint, quoi qu'il arrive
        if (relayState) {
            setRelay(false);
        }
        
    } 
    else if (currentMode == AUTO) {
        // Vérifie si on est bloqué par le mode Jour/Nuit
        bool blockedByDaylight = (dayNightEnabled && isDayTime);
        
        if (!blockedByDaylight && !waitForPirLow) {
            if (pirState && !relayState) {
                // Nouvelle détection : On allume et on lance le timer
                setRelay(true);
                startTimer(autoTimerMinutes);
            } 
            else if (pirState && relayState && autoTimerMinutes > 0) {
                // Mouvement continu : On réinitialise le timer pour prolonger l'allumage
                startTimer(autoTimerMinutes);
            }
        }
        
    } 
    else if (currentMode == MANUAL) {
        // En mode manuel, le relais ne réagit qu'aux ordres web (géré dans handleHardwareAction).
        // On ne fait rien ici, la minuterie s'occupe de l'extinction si besoin.
    }
}

void handleHardwareAction(String action, String strValue, int intValue) {
    if (action == "TOGGLE_RELAY") {
        bool oldState = relayState;
        bool newState = !relayState;
        setRelay(newState);
        
        if (newState) {
            startTimer(manualTimerMinutes);
            if (currentMode == AUTO) waitForPirLow = true;
        } else {
            startTimer(0);
            if (currentMode == AUTO) waitForPirLow = true;
        }
        
        Serial.printf("[HARDWARE] Action: TOGGLE_RELAY | Relais: %s -> %s (Timer: %d min)\n", 
                      oldState ? "ON" : "OFF", newState ? "ON" : "OFF", newState ? manualTimerMinutes : 0);
        
    } 
    else if (action == "SET_MODE") {
        // On sauvegarde l'ancien mode pour le log
        String oldModeStr = (currentMode == MANUAL) ? "MANUEL" : (currentMode == AUTO) ? "AUTO" : "ALARME";
        
        if (strValue == "MANUAL") currentMode = MANUAL;
        else if (strValue == "AUTO") currentMode = AUTO;
        else if (strValue == "ALARM") currentMode = ALARM;
        
        setRelay(false);
        startTimer(0);
        waitForPirLow = false;
        
        Serial.printf("[HARDWARE] Action: SET_MODE | Mode: %s -> %s\n", oldModeStr.c_str(), strValue.c_str());
        
    } 
    else if (action == "TOGGLE_DAYNIGHT") {
        bool oldDn = dayNightEnabled;
        dayNightEnabled = !dayNightEnabled;
        
        Serial.printf("[HARDWARE] Action: TOGGLE_DAYNIGHT | Contrainte J/N: %s -> %s\n", 
                      oldDn ? "ON" : "OFF", dayNightEnabled ? "ON" : "OFF");
        
    } 
    else if (action == "SET_MANUAL_TIMER") {
        if (intValue < 0) intValue = 0;
        if (intValue > 180) intValue = 180;
        manualTimerMinutes = intValue;
        
        Serial.printf("[HARDWARE] Action: SET_MANUAL_TIMER | Valeur: %d min\n", manualTimerMinutes);
        
    } 
    else if (action == "SET_AUTO_TIMER") {
        if (intValue < 1) intValue = 1;
        if (intValue > 60) intValue = 60;
        autoTimerMinutes = intValue;
        
        Serial.printf("[HARDWARE] Action: SET_AUTO_TIMER | Valeur: %d min\n", autoTimerMinutes);
    }
}