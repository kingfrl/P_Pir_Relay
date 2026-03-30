#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <PubSubClient.h>

extern PubSubClient mqttClient;

void setupMQTT();
void handleMQTT();
void publishRelayState(bool state);

#endif