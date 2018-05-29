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
    DynamicJsonBuffer jsonBuffer;
    IoT_LED* controlLED;
    std::vector<IoT_Control*> controls;
    std::map<const String, IoT_Control*> controlMap;
    bool setupComplete;

  public:
    IoT_Server(const std::initializer_list<IoT_Control*> controls)
        : IoT_Server(115200, controls) {
    }

    IoT_Server(unsigned long baudRate, const std::initializer_list<IoT_Control*> controls)
        : webServer(80)
        , webSocket(8080)
        , jsonBuffer(JSON_OBJECT_SIZE(controls.size()) + 100)
        , controls(controls)
        , setupComplete(false) {

        Serial.begin(baudRate);

        for (IoT_Control* control : controls) {
            controlMap[control->getName()] = control;
        }

        controlLED = findControlLED();

        debugLine("Starting filesystem...");
        SPIFFS.begin();

        debugLine("Web server basic setup...");
        webServer.onNotFound([&]() { handleFiles(); });
        webServer.on("/get", HTTP_GET, [&]() { getValues(); });
        webServer.on("/get", HTTP_OPTIONS, [&]() { getOptions(); });
        webServer.on("/set", HTTP_POST, [&]() { setValues(); });
        webServer.on("/set", HTTP_OPTIONS, [&]() { getOptions(); });
    }

    void setup(const char* ssid, const char* password) {
        debugLine("Initializing controls:");
        for (IoT_Control* control : controls) {
            control->setup();
            debugLine(control->getName());
        }

        enableControlLED();
        setupWifi(ssid, password);
        debugLine("Starting web server.");
        webServer.begin();
        debugLine("Starting web socket.");
        webSocket.begin();
        printUrl();
        disableControlLED();
        setupComplete = true;
    }

    void loop() {
        for (IoT_Control* control : controls) {
            int status = control->loop();
            if (status != IOT_STATUS_UNCHANGED) {
                JsonObject& notification = createResponse();
                notification[control->getName()] = status;
                sendNotification(notification);
            }
        }
        webSocket.loop();
        webServer.handleClient();
    }

    void handleRequest(const String& uri, const HTTPMethod method, const std::function<void(void)> callback) {
        debugLine("Handling request: " + uri + ", " + getMethodName(method));
        webServer.on(uri, method, callback);
        if (method != HTTP_OPTIONS) {
            webServer.on(uri, HTTP_OPTIONS, [&]() { getOptions(); });
        }
    }

    void sendNotification(const JsonObject& response) {
        String notification = getJsonString(response);
        debugLine("Notification: " + notification);
        webSocket.broadcastTXT(notification);
    }

    void sendResponse(const JsonObject& response) {
        String responseString = getJsonString(response);
        debugLine("Response: " + responseString);
        webServer.sendHeader("Access-Control-Allow-Origin", "*");
        webServer.send(200, "application/json", responseString);
    }

    JsonObject& createResponse() {
        return jsonBuffer.createObject();
    }

    const String getParameter(const String name) {
        return webServer.arg(name);
    }

    const int setValue(const String name, const int value) {
        std::map<String, IoT_Control*>::iterator it = controlMap.find(name);
        if (it == controlMap.end()) {
            return 0;
        }

        int result = it->second->setValue(value);

        JsonObject& notification = createResponse();
        notification[it->first] = result;
        sendNotification(notification);

        return result;
    }

    const int getValue(const String name) {
        std::map<String, IoT_Control*>::iterator it = controlMap.find(name);
        if (it == controlMap.end()) {
            return 0;
        }

        return it->second->getValue();
    }

    virtual ~IoT_Server() {
        for (IoT_Control* control : controls) {
            delete control;
        }
        controls.clear();
        controlMap.clear();
        controlLED = NULL;
    }

  private:
    void debug(const String text) {
#ifdef IoT_DEBUG
        Serial.print(text);
#endif
    }

    void debugLine(const String text) {
#ifdef IoT_DEBUG
        Serial.println(text);
#endif
    }

    IoT_LED* findControlLED() {
        std::map<String, IoT_Control*>::iterator it = controlMap.find(IoT_Control_LED);
        if (it != controlMap.end()) {
            return (IoT_LED*) it->second;
        }
        return NULL;
    }

    void enableControlLED() {
        if (controlLED) {
            controlLED->enable();
            if (!setupComplete) {
                controlLED->loop();
            }
        }
    }

    void disableControlLED() {
        if (controlLED) {
            controlLED->disable();
            if (!setupComplete) {
                controlLED->loop();
            }
        }
    }

    void blinkControlLED() {
        if (controlLED) {
            controlLED->blink();
            if (!setupComplete) {
                controlLED->loop();
            }
        }
    }

    const String getMethodName(const HTTPMethod method) {
        switch (method) {
            case HTTP_ANY:
                return "ANY";
            case HTTP_GET:
                return "GET";
            case HTTP_POST:
                return "POST";
            case HTTP_PUT:
                return "PUT";
            case HTTP_PATCH:
                return "PATCH";
            case HTTP_DELETE:
                return "DELETE";
            case HTTP_OPTIONS:
                return "OPTIONS";
        }
    }

    void setupWifi(const char* ssid, const char* password) {
        Serial.print("Connecting to ");
        Serial.print(ssid);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            blinkControlLED();
        }
        enableControlLED();
        Serial.println("");
        Serial.println("WiFi connected.");
    }

    void handleFiles() {
        enableControlLED();
        String path = webServer.uri();
        if (path.endsWith("/")) {
            path += "index.html";
        }

        if (!SPIFFS.exists(path)) {
            webServer.send(404, "text/plain", "404: File Not Found");
            return;
        }

        File file = SPIFFS.open(path, "r");
        webServer.streamFile(file, getContentType(path));
        file.close();
        disableControlLED();
    }

    const String getContentType(const String filename) {
        if (filename.endsWith(".html")) {
            return "text/html";
        } else if (filename.endsWith(".css")) {
            return "text/css";
        } else if (filename.endsWith(".js")) {
            return "application/javascript";
        } else if (filename.endsWith(".png")) {
            return "image/png";
        } else if (filename.endsWith(".jpg")) {
            return "image/jpeg";
        }

        return "text/plain";
    }

    void getOptions() {
        enableControlLED();
        webServer.sendHeader("Access-Control-Allow-Origin", "*");
        webServer.sendHeader("Access-Control-Max-Age", "10000");
        webServer.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
        webServer.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
        webServer.send(200, "text/plain");
        disableControlLED();
    }

    const String getJsonString(const JsonObject& response) {
        String responseString = String();
        response.printTo(responseString);

#ifdef IoT_DEBUG
        response.printTo(Serial);
        Serial.println("");
#endif

        return responseString;
    }

    void printUrl() {
        Serial.print("URL: ");
        Serial.print("http://");
        Serial.print(WiFi.localIP());
        Serial.println("/");
    }

    void getValues() {
        JsonObject& response = createResponse();
        for (IoT_Control* control : controls) {
            response[control->getName()] = control->getValue();
        }
        sendResponse(response);
    }

    void setValues() {
        JsonObject& response = createResponse();
        for (IoT_Control* control : controls) {
            if (webServer.hasArg(control->getName())) {
                response[control->getName()] = control->setValue(webServer.arg(control->getName()).toInt());
            }
        }
        sendResponse(response);
    }
};
#endif // IOT_SERVER_H
