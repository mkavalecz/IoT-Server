#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#ifdef IOT_TRACE
#define IOT_DEBUG
#endif

#define IOT_CONTROL_LED "CONTROL_LED"

#ifndef IOT_NUM_SSID
#define IOT_NUM_SSID 5
#endif

#ifndef IOT_WIFI_CONFIG
#define IOT_WIFI_CONFIG "/config/wifi.conf"
#endif

#ifndef IOT_AUTH_CONFIG
#define IOT_AUTH_CONFIG "/config/auth.conf"
#endif

#ifndef IOT_TITLE
#define IOT_TITLE "IoT-Server"
#endif

#ifndef IOT_BAUD_RATE
#define IOT_BAUD_RATE 115200
#endif

#include "Arduino.h"
#include <map>
#include <FS.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "controls/IoT_Control.h"
#include "controls/IoT_Slider.h"

class IoT_Server {
  private:
    ESP8266WebServer webServer;
    WebSocketsServer webSocket;
    IoT_Slider* controlLED;
    std::vector<IoT_Control*> controls;
    std::map<const char*, IoT_Control*> controlMap;
    String userName;
    String password;
    bool setupComplete;
    size_t bufferSize;

  public:
    IoT_Server(const std::initializer_list<IoT_Control*> controls);

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
    void debug(const char* text);
    void debugLine(const char* text);
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
};
#endif // IOT_SERVER_H
