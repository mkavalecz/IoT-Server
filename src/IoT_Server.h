#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#ifdef IoT_TRACE
#define IoT_DEBUG
#endif

#define IoT_Control_LED "CONTROL_LED"

#include <map>
#include <FS.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "controls/IoT_Control.h"
#include "controls/IoT_LED.h"

class IoT_Server {
  private:
    ESP8266WebServer webServer;
    WebSocketsServer webSocket;
    IoT_LED* controlLED;
    std::vector<IoT_Control*> controls;
    std::map<const String, IoT_Control*> controlMap;
    bool setupComplete;
    size_t bufferSize;

  public:
    IoT_Server(const std::initializer_list<IoT_Control*> controls)
        : IoT_Server(115200, controls) {
    }
    IoT_Server(unsigned long baudRate, const std::initializer_list<IoT_Control*> controls);

    void setup(const char* ssid, const char* password);
    void loop();

    void handleRequest(const String& uri, const HTTPMethod method, const std::function<void(void)> callback);

    void sendNotification(const DynamicJsonDocument& response);
    void sendResponse(const DynamicJsonDocument& response);

    const String getParameter(const String id);

    const int getValue(const String id);
    const int setValue(const String id, const int value);

    virtual ~IoT_Server();

  private:
    void debugLine(const String text);
    const String getJsonString(const DynamicJsonDocument& response);

    IoT_LED* findControlLED();
    void enableControlLED();
    void disableControlLED();
    void blinkControlLED();

    void setupWifi(const char* ssid, const char* password);
    void printUrl();

    void handleFiles();
    const String getContentType(const String filename);
    const String getMethodName(const HTTPMethod method);
    void sendOptionsHeaders();

    void getControls();
    void setControls();
};
#endif // IOT_SERVER_H
