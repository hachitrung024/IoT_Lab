#ifndef THINGSBOARD_TASK_H
#define THINGSBOARD_TASK_H
#include "config.h"
#include <WiFi.h>
#include <ThingsBoard.h>
#include <Server_Side_RPC.h>
#include <Arduino_MQTT_Client.h>
#include <Shared_Attribute_Update.h>
#include <Attribute_Request.h>
#include <OTA_Firmware_Update.h>
#include <Espressif_Updater.h>
#include "tasks/send_task.h"
void thingsboard_task(void * pvParameter);
void InitWiFi();
bool reconnect(); 
void handleRequest(const JsonVariantConst &data, JsonDocument &response);
void processSharedAttributeUpdate(const JsonObjectConst &data);
void processSharedAttributeRequest(const JsonObjectConst &data);
void requestTimedOut();
void update_starting_callback();
void finished_callback(const bool & success);
void progress_callback(const size_t & current, const size_t & total);
void checkForUpdate(void * pvParameters);
#endif //THINGSBOARD_TASK_H