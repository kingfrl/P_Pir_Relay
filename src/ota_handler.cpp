#include "ota_handler.h"
#include <ElegantOTA.h>
#include "config.h"

void setupOTA() {
    ElegantOTA.begin(iotWebConf.getWebServer());
    Serial.println("ElegantOTA activé → http://IP/ota");
}

void handleOTA() {
    ElegantOTA.loop();
}