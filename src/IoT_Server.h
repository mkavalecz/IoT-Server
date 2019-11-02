#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#define IOT_CONTROL_LED "CONTROL_LED"

#define IOT_NUM_SSID 5
#define IOT_WIFI_CONFIG "/config/wifi.conf"
#define IOT_AUTH_CONFIG "/config/auth.conf"
#define IOT_DATA_PATH "/data/"
#define IOT_BAUD_RATE 115200

#include "Arduino.h"
#include <map>
#include <FS.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "controls/IoT_Control.h"
#include "controls/IoT_Slider.h"
#include "debug.h"

class IoT_Server {
  private:
    ESP8266WebServer webServer;
    WebSocketsServer webSocket;
    IoT_Slider* controlLED;
    std::vector<IoT_Control*> controls;
    std::map<const char*, IoT_Control*> controlMap;
    const char* title;
    String userName;
    String password;
    bool setupComplete;
    size_t bufferSize;

  public:
    IoT_Server(const std::initializer_list<IoT_Control*> controls)
        : IoT_Server("IoT-Server", controls) {
    }

    IoT_Server(const char* title, const std::initializer_list<IoT_Control*> controls);

    void setup();
    void loop();

    void handleRequest(const char* uri, const HTTPMethod method, const std::function<void(void)> callback);

    bool checkAuthentication();

    void sendNotification(const DynamicJsonDocument& response);
    void sendResponse(const DynamicJsonDocument& response);

    const char* getParameter(const char* id);

    const int getValue(const char* id);
    const int setValue(const char* id, const int value);

    virtual ~IoT_Server();

  private:
    const String getJsonString(const DynamicJsonDocument& response);

    IoT_Slider* findControlLED();
    void enableControlLED();
    void disableControlLED();
    void blinkControlLED();

    void setupControls();
    void setupWifi();
    void setupAuthentication();
    void setupWebServer();
    void setupWebSocket();
    void printUrl();

    void handleFiles();
    const char* getContentType(const String& filename);
    const char* getMethodName(const HTTPMethod method);
    void sendOptionsHeaders();

    void getTitle();
    void getControls();
    void setControls();
    void saveControls();
};
#endif // IOT_SERVER_H
