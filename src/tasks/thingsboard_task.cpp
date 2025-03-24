#include "thingsboard_task.h"
#ifdef USE_MQTT
//Shared Attributes Configuration
constexpr uint8_t MAX_ATTRIBUTES = 5;
constexpr std::array<const char*, MAX_ATTRIBUTES> 
SHARED_ATTRIBUTES = 
{
    "rgb_bright",
    "rgb_r",
    "rgb_g",
    "rgb_b",
    "sendInterval"
};
// Initialize the Ethernet client object
WiFiClient wifiClient;
// Initalize the Mqtt client instance
Arduino_MQTT_Client mqttClient(wifiClient);
// Initialize used apis
Server_Side_RPC<MAX_RPC_SUBSCRIPTIONS, MAX_RPC_RESPONSE> rpc;
Shared_Attribute_Update<1U, MAX_ATTRIBUTES> shared_update;
Attribute_Request<2U, MAX_ATTRIBUTES> attr_request;
OTA_Firmware_Update<> ota;
IAPI_Implementation* apis[4] = {
&rpc,
&shared_update,
&attr_request,
&ota
};
// Initialize ThingsBoard instance with the maximum needed buffer sizes
ThingsBoard tb(mqttClient, MAX_MESSAGE_RECEIVE_SIZE, MAX_MESSAGE_SEND_SIZE, Default_Max_Stack_Size, apis + 0U, apis + 4U);
Espressif_Updater<> updater;
// Statuses for updating
void thingsboard_task(void * pvParameter){
    // Statuses for subscribing to rpc
    bool rpc_subscribed = false;
    bool shared_update_subscribed = false;
    bool requestedShared = false;
    bool currentFWSent = false;
    bool updateRequestSent = false;
    InitWiFi();
    // tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT);
    xTaskCreate(send_task, "Send Data", 4096, &tb, 2, NULL);
    for(;;){
        delay(1000);
        if(!reconnect())
            continue;
        if (!tb.connected()){
            char message[Helper::detectSize(CONNECTING_MSG, THINGSBOARD_SERVER, THINGSBOARD_TOKEN)];
            snprintf(message, sizeof(message), CONNECTING_MSG, THINGSBOARD_SERVER, THINGSBOARD_TOKEN);
            Serial.println(message);
            if (!tb.connect(THINGSBOARD_SERVER, THINGSBOARD_TOKEN, THINGSBOARD_PORT)) {
                Serial.println("Failed to connect");
                delay(5000);
                continue;
            }
        }
        if (!currentFWSent) {
            currentFWSent = ota.Firmware_Send_Info(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION);
        }
        if (!updateRequestSent) {
            Serial.print(CURRENT_FIRMWARE_TITLE);
            Serial.println(CURRENT_FIRMWARE_VERSION);
            const OTA_Update_Callback callback(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION, &updater, &finished_callback, &progress_callback, &update_starting_callback, FIRMWARE_FAILURE_RETRIES, FIRMWARE_PACKET_SIZE);
            Serial.println("Firwmare Update Subscription...");
            updateRequestSent = ota.Subscribe_Firmware_Update(callback);
            if(updateRequestSent) xTaskCreate(checkForUpdate, "CheckUpdate", 2048, NULL, 5, NULL);
          }
        if (!rpc_subscribed){
            Serial.println("Subscribing for RPC...");
            const RPC_Callback callbacks[MAX_RPC_SUBSCRIPTIONS]= {
                {"rpc_method", handleRequest}
            };
            if (!rpc.RPC_Subscribe(callbacks + 0U, callbacks + MAX_RPC_SUBSCRIPTIONS)) {
                Serial.println("Failed to subscribe for RPC");
                continue;
            }
            Serial.println("Subscribe done");
            rpc_subscribed = true;
        }
        if (!shared_update_subscribed){
            Serial.println("Subscribing for shared attribute updates...");
            const Shared_Attribute_Callback<MAX_ATTRIBUTES> callback(&processSharedAttributeUpdate, SHARED_ATTRIBUTES);
            if (!shared_update.Shared_Attributes_Subscribe(callback)) {
            Serial.println("Failed to subscribe for shared attribute updates");
            continue;
            }
            Serial.println("Subscribe done");
            shared_update_subscribed = true;
        }
        if (!requestedShared) {
            Serial.println("Requesting shared attributes...");
            const Attribute_Request_Callback<MAX_ATTRIBUTES> sharedCallback(&processSharedAttributeRequest, REQUEST_TIMEOUT_MICROSECONDS, &requestTimedOut, SHARED_ATTRIBUTES);
            requestedShared = attr_request.Shared_Attributes_Request(sharedCallback);
            if (!requestedShared) {
              Serial.println("Failed to request shared attributes");
            }
        }
        tb.loop();
    }
}
void handleRequest(const JsonVariantConst &data, JsonDocument &response){
    Serial.println("Received a RPC request");
    //Info
    const size_t jsonSize = Helper::Measure_Json(data);
    char buffer[jsonSize];
    serializeJson(data, buffer, jsonSize);
    Serial.println(buffer);
}
void InitWiFi() {
    Serial.println("Connecting to WiFi...");
    // Attempting to establish a connection to the given WiFi network
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        // Delay 500ms until a connection has been successfully established
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}
bool reconnect() {
    // Check to ensure we aren't connected yet
    const uint8_t status = WiFi.status();
    if (status == WL_CONNECTED) {
        return true;
    }

    // If we aren't establish a new connection to the given WiFi network
    InitWiFi();
    return true;
}
void processSharedAttributeUpdate(const JsonObjectConst &data) {
    // if(data.containsKey("rgb_bright")){
    //     shared.rgb_bright = int(data["rgb_bright"] )* 255 / 100;
    //     if(shared.rgb_bright <= 0) shared.rgb_bright = 1;

    // }
    // if(data.containsKey("sendInterval"))
    //     shared.sendInterval = int(data["sendInterval"]);
        
    //Info
    const size_t jsonSize = Helper::Measure_Json(data);
    char buffer[jsonSize];
    serializeJson(data, buffer, jsonSize);
    Serial.println(buffer);
}
void processSharedAttributeRequest(const JsonObjectConst &data) {
    // shared.rgb_bright = int(data["rgb_bright"] )* 255 / 100;
    // if(shared.rgb_bright <= 0) shared.rgb_bright = 1;

    // shared.sendInterval = int(data["sendInterval"]);
    //Info
    const size_t jsonSize = Helper::Measure_Json(data);
    char buffer[jsonSize];
    serializeJson(data, buffer, jsonSize);
    Serial.println(buffer);
}
void requestTimedOut() {
    Serial.printf("Attribute request timed out did not receive a response in (%llu) microseconds. Ensure client is connected to the MQTT broker and that the keys actually exist on the target device\n", REQUEST_TIMEOUT_MICROSECONDS);
}
void update_starting_callback() {
}
void finished_callback(const bool & success) {
  if (success) {
    Serial.println("Done, Reboot now");
    esp_restart();
    return;
  }
  Serial.println("Downloading firmware failed");
}
void progress_callback(const size_t & current, const size_t & total) {
  Serial.printf("Progress %.2f%%\n", static_cast<float>(current * 100U) / total);
}
void checkForUpdate(void * pvParameters){
    vTaskDelay(30000);
    const OTA_Update_Callback callback(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION, &updater, &finished_callback, &progress_callback, &update_starting_callback, FIRMWARE_FAILURE_RETRIES, FIRMWARE_PACKET_SIZE);
    Serial.println("Firwmare Update ...");
    ota.Start_Firmware_Update(callback);
    vTaskDelete(NULL);
  }
#endif //USE_MQTT