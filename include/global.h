#ifndef GLOBAL_H
#define GLOBAL_H
#include <Arduino.h>
#include <SPI.h>

#define SPI2_SCK    SCK
#define SPI2_MISO   -1
#define SPI2_MOSI   MOSI
#define SPI2_SS     -1

#define SPI3_SCK    12
#define SPI3_MISO   14
#define SPI3_MOSI   13
#define SPI3_SS     11
extern SPIClass spi2;
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