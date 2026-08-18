#include <Arduino.h>
#include "Config.h"
#include "Storage.h"
#include "Network.h"
#include "Astro.h"
#include "Hardware.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    initFileSystem();

    // Chargement de l'état du mode Jour/Nuit (vaut true par défaut si non enregistré)
    bool dayNightEnabled = true;
    loadDayNightState(dayNightEnabled);

    initHardware(); 

    String savedSSID, savedPASS;
    if (loadCredentials(savedSSID, savedPASS)) {
        Serial.println("Identifiants trouvés pour : " + savedSSID);
        WiFi.mode(WIFI_STA); 
        startSilentConnection(savedSSID, savedPASS);
    } else {
        startAPMode();
    }
}

void loop() {
    loopNetwork();
    updateHardware(); // (DOIT ÊTRE RAPIDE, SANS DELAY)
    
    if (WiFi.status() == WL_CONNECTED) {
        static bool astroInitialized = false;
        if (!astroInitialized) {
            initAstro();
            astroInitialized = true;
        }
        updateAstroState();
    }
}