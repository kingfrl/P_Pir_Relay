#include <Arduino.h>
#include "wifi_manager.h"
#include "ota_handler.h"
#include "pir_sensor.h"
#include "relay_control.h"
#include "mqtt_client.h"
#include "web_interface.h"
#include "time_utils.h"

void setup() {
    Serial.begin(115200);
    setupWiFi();
    setupOTA();
    setupPIR();
    setupRelay();
    setupMQTT();
    setupWebInterface();
    setupTimeUtils();
}

void loop() {
    handleWiFi();
    handleOTA();
    handlePIR();
    handleRelay();
    handleMQTT();
    handleWebInterface();
}