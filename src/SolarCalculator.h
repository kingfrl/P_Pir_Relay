#ifndef SOLAR_CALCULATOR_H
#define SOLAR_CALCULATOR_H

#include <Arduino.h>

class SolarCalculator {
public:
    void setPosition(double latitude, double longitude);
    double calcSunrise(int year, int month, int day);
    double calcSunset(int year, int month, int day);
};

#endif