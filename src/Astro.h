#pragma once
#include <Arduino.h>

// La variable publique qui dit si c'est le jour ou la nuit
extern bool isDayTime;

void initAstro();
void updateAstroState();