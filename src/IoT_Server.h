#ifndef IOT_SERVER_H
#define IOT_SERVER_H

#ifdef IoT_TRACE
#define IoT_DEBUG
#endif

#define IOT_CONTROL_LED "CONTROL_LED"

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
    std::map<const String, IoT_Control*> controlMap;
    String userName;
    String password;
    String title;
    bool setupComplete;
    size_t bufferSize;

  public:
    IoT_Server(const std::initializer_list<IoT_Control*> controls)
        : IoT_Server(115200, "admin", "admin", "IoT-Server", controls) {
    }

    IoT_Server(const String title, const std::initializer_list<IoT_Control*> controls)
        : IoT_Server(115200, "admin", "admin", title, controls) {
    }

    IoT_Server(const String userName, const String password, const String title,
        const std::initializer_list<IoT_Control*> controls)
        : IoT_Server(115200, userName, password, title, controls) {
    }

    IoT_Server(unsigned long baudRate, const String userName, const String password, const String title,
        const std::initializer_list<IoT_Control*> controls);

    void setup(String ssid, String password) {
        setup(ssid, password, "", "");
    }

    void setup(String ssid1, String password1, String ssid2, String password2);
    void loop();

    void handleRequest(const String& uri, const HTTPMethod method, const std::function<void(void)> callback);

    bool checkAuthentication();

    void sendNotification(const DynamicJsonDocument& response);
    void sendResponse(const DynamicJsonDocument& response);

    const String getParameter(const String id);

    const int getValue(const String id);
    const int setValue(const String id, const int value);

    virtual ~IoT_Server();

  private:
    void debugLine(const String text);
    const String getJsonString(const DynamicJsonDocument& response);

    IoT_Slider* findControlLED();
    void enableControlLED();
    void disableControlLED();
    void blinkControlLED();

    void setupWifi(String ssid, String password);
    void setupWifi(String ssid1, String password1, String ssid2, String password2);
    void printUrl();

    void handleFiles();
    const String getContentType(const String filename);
    const String getMethodName(const HTTPMethod method);
    void sendOptionsHeaders();

    void getTitle();
    void getControls();
    void setControls();
};
#endif // IOT_SERVER_H
