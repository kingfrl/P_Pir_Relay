#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

void setupWebInterface();
void handleWebInterface();

#endif