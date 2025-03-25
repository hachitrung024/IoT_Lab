#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
//Serial configuration
constexpr unsigned int SERIAL_BAUD = 115200;
// Device ID
constexpr char DEVICE_NAME[] = "esp32s3";
// Firmware title and version used to compare with remote version, to check if an update is needed.
// Title needs to be the same and version needs to be different --> downgrading is possible
constexpr char CURRENT_FIRMWARE_TITLE[] = "TB";
constexpr char CURRENT_FIRMWARE_VERSION[] = "0.0.0";
//WiFi configuaration
constexpr char WIFI_SSID[] = "OXY 24H 2.4G";
constexpr char WIFI_PASSWORD[] = "oxy24hcoffee";
// MQTT
// #define CONFIG_THINGSBOARD_ENABLE_DEBUG true
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr char THINGSBOARD_TOKEN[] =  "yxg2bvgmm2cu4crmf3ch";

constexpr uint16_t MAX_MESSAGE_SEND_SIZE = 128U;
constexpr uint16_t MAX_MESSAGE_RECEIVE_SIZE = 128U;

//RPC configuration
constexpr uint8_t MAX_RPC_SUBSCRIPTIONS = 1U;
constexpr uint8_t MAX_RPC_RESPONSE = 5U;
constexpr char CONNECTING_MSG[] = "Connecting to: (%s) with token (%s)";
constexpr char RPC_METHOD[] = "rpc_method";

// Maximum amount of attributs we can request or subscribe, has to be set both in the ThingsBoard template list and Attribute_Request_Callback template list
// and should be the same as the amount of variables in the passed array. If it is less not all variables will be requested or subscribed

char constexpr RGB_BRIGHT[] = "rgb_bright";
char constexpr RGB_R[] = "rgb_r";
char constexpr RGB_G[] = "rgb_g";
char constexpr RGB_B[] = "rgb_b";
constexpr uint64_t REQUEST_TIMEOUT_MICROSECONDS = 10000U * 1000U;

// Maximum amount of retries we attempt to download each firmware chunck over MQTT
constexpr uint8_t FIRMWARE_FAILURE_RETRIES = 12U;
// Size of each firmware chunck downloaded over MQTT,
// increased packet size, might increase download speed
constexpr uint16_t FIRMWARE_PACKET_SIZE = 4096U;

// #include "pins_yolobit.h"
#define USE_MQTT
// #define DHT_PIN 6
#define USE_I2C
#define USE_LCD1602
#define USE_DHT20
// #define PUMP1_PIN P10
// #define PUMP2_PIN P13
// #define RGB_PIN         45
// #define ADC_MOIS_PIN    P1_ADC
// #define ADC_LIGHT_PIN   P2_ADC
#ifdef USE_I2C
#define I2C_SDA 11
#define I2C_SCL 12
#endif //USE_I2C
#endif //CONFIG_H