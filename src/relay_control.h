#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

void setupRelay();
void setRelay(bool state);
bool getRelayState();
void handleRelay();
unsigned long getActiveDuration();  // Ajoute cette ligne

#endif