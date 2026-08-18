#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

extern DNSServer dnsServer;
extern ESP8266WebServer server;

enum State { IDLE, CONNECTING, CONNECTED, FAILED };
extern State currentState;

void startAPMode();
void startNormalMode();
void startSilentConnection(String ssid, String pass);
void loopNetwork();

// --- API JSON (Pour la page Web) ---
void handleApiAction();

// --- API REST (Pour les scripts externes) ---
void handleUrlStatus();
void handleUrlRelaisOn();
void handleUrlRelaisOff();
void handleUrlSetMode();
void handleUrlJourOn();
void handleUrlJourOff();
void handleUrlTimerManual();
void handleUrlTimerAuto();