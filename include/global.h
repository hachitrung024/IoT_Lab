#ifndef GLOBAL_H
#define GLOBAL_H
#include <Arduino.h>
struct TelemetryData {
    float temperature;
    float humidity;
    float pressure;
    unsigned long lastUpdateTime;
};
struct SharedData {
    uint8_t rgb_bright;
    uint8_t rgb_r;
    uint8_t rgb_g;
    uint8_t rgb_b;
    unsigned int sendInterval;
};
extern volatile TelemetryData telemetry;
extern volatile SharedData shared;
void updateRGB();
#endif