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

  // Récupérer la ville sélectionnée
  const char* cityInfo = cityParam.getValueBuffer();

  // Passer la ville sélectionnée à setupTimeUtils
  setupTimeUtils(cityInfo);
}

void loop() {
  handleWiFi();
  handleOTA();
  handlePIR();
  handleRelay();
  handleMQTT();
  handleWebInterface();
}