#include "mqtt_client.h"
#include "relay_control.h"
#include <ESP8266WiFi.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setupMQTT() {
  mqttClient.setServer("192.168.1.100", 1883);
  mqttClient.setCallback([](char* topic, byte* payload, unsigned int length) {
    // Gestion des messages MQTT reçus
  });
}

void handleMQTT() {
  if (!mqttClient.connected()) {
    mqttClient.connect("Projecteur-X2");
  }
  mqttClient.loop();
}

void publishRelayState(bool state) {
  mqttClient.publish("home/projecteur/relay", state ? "ON" : "OFF");
}