#include "mqtt_client.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "relay_control.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* mqtt_server = "broker.hivemq.com";  // Change si tu as ton propre broker
const int mqtt_port = 1883;

void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.printf("MQTT Message [%s]: %s\n", topic, message.c_str());

    if (String(topic) == "projecteur/command") {
        if (message == "ON") {
            manualOverride = true;
            turnRelayOn();
        } else if (message == "OFF") {
            manualOverride = false;
            turnRelayOff();
        }
    }
}

void setupMQTT() {
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(callback);
}

void reconnect() {
    if (WiFi.status() == WL_CONNECTED) {
        String clientId = "ProjecteurX2-" + String(random(0xffff), HEX);
        if (mqttClient.connect(clientId.c_str())) {
            Serial.println("MQTT connecté");
            mqttClient.subscribe("projecteur/command");
        }
    }
}

void handleMQTT() {
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
}