#include "IoT_Server.h"

IoT_Server::IoT_Server(unsigned long baudRate, const std::initializer_list<IoT_Control*> controls)
    : webServer(80)
    , webSocket(8080)
    , controls(controls)
    , setupComplete(false)
    , bufferSize(JSON_OBJECT_SIZE(controls.size()) + (controls.size() * JSON_OBJECT_SIZE(10)) + 100) {

    Serial.begin(baudRate);

    for (IoT_Control* control : controls) {
        controlMap[control->getId()] = control;
    }

    controlLED = findControlLED();

    debugLine("Starting filesystem...");
    SPIFFS.begin();

    debugLine("Web server basic setup...");
    webServer.onNotFound([&]() { handleFiles(); });
    webServer.on("/get", HTTP_GET, [&]() { getControls(); });
    webServer.on("/get", HTTP_OPTIONS, [&]() { sendOptionsHeaders(); });
    webServer.on("/set", HTTP_POST, [&]() { setControls(); });
    webServer.on("/set", HTTP_OPTIONS, [&]() { sendOptionsHeaders(); });
}

void IoT_Server::setup(const char* ssid, const char* password) {
    debugLine("Initializing controls:");
    for (IoT_Control* control : controls) {
        control->setup();
        debugLine(control->getId() + " (" + control->getName() + ")");
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

void IoT_Server::loop() {
    DynamicJsonBuffer buffer(bufferSize);
    for (IoT_Control* control : controls) {
        int status = control->loop();
        if (status != IOT_STATUS_UNCHANGED) {
            JsonObject& notification = buffer.createObject();
            control->serializeJsonTo(notification);
            sendNotification(notification);
        }
    }
    webSocket.loop();
    webServer.handleClient();
}

void IoT_Server::handleRequest(const String& uri, const HTTPMethod method, const std::function<void(void)> callback) {
    debugLine("Handling request: " + uri + ", " + getMethodName(method));
    webServer.on(uri, method, callback);
    if (method != HTTP_OPTIONS) {
        webServer.on(uri, HTTP_OPTIONS, [&]() { sendOptionsHeaders(); });
    }
}

void IoT_Server::sendNotification(const JsonObject& response) {
    String notification = getJsonString(response);
    debugLine("Notification: " + notification);
    webSocket.broadcastTXT(notification);
}

void IoT_Server::sendResponse(const JsonObject& response) {
    String responseString = getJsonString(response);
    debugLine("Response: " + responseString);
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", responseString);
}

const String IoT_Server::getParameter(const String id) {
    return webServer.arg(id);
}

const int IoT_Server::getValue(const String id) {
    std::map<String, IoT_Control*>::iterator it = controlMap.find(id);
    if (it == controlMap.end()) {
        return 0;
    }

    return it->second->getValue();
}

const int IoT_Server::setValue(const String id, const int value) {
    std::map<String, IoT_Control*>::iterator it = controlMap.find(id);
    if (it == controlMap.end()) {
        return 0;
    }

    int result = it->second->setValue(value);

    DynamicJsonBuffer buffer(bufferSize);
    JsonObject& notification = buffer.createObject();
    it->second->serializeJsonTo(notification);
    sendNotification(notification);

    return result;
}

IoT_Server::~IoT_Server() {
    for (IoT_Control* control : controls) {
        delete control;
    }
    controls.clear();
    controlMap.clear();
    controlLED = NULL;
}

void IoT_Server::debugLine(const String text) {
#ifdef IoT_DEBUG
    Serial.println(text);
#endif
}

const String IoT_Server::getJsonString(const JsonObject& response) {
    String responseString = String();
    response.printTo(responseString);

#ifdef IoT_DEBUG
    response.printTo(Serial);
    Serial.println("");
#endif

    return responseString;
}

IoT_LED* IoT_Server::findControlLED() {
    std::map<String, IoT_Control*>::iterator it = controlMap.find(IoT_Control_LED);
    if (it != controlMap.end()) {
        return (IoT_LED*) it->second;
    }
    return NULL;
}

void IoT_Server::enableControlLED() {
    if (controlLED) {
        controlLED->enable();
        if (!setupComplete) {
            controlLED->loop();
        }
    }
}

void IoT_Server::disableControlLED() {
    if (controlLED) {
        controlLED->disable();
        if (!setupComplete) {
            controlLED->loop();
        }
    }
}

void IoT_Server::blinkControlLED() {
    if (controlLED) {
        controlLED->blink();
        if (!setupComplete) {
            controlLED->loop();
        }
    }
}

void IoT_Server::setupWifi(const char* ssid, const char* password) {
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

void IoT_Server::printUrl() {
    Serial.print("URL: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void IoT_Server::handleFiles() {
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
}

const String IoT_Server::getContentType(const String filename) {
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
    } else if (filename.endsWith(".gif")) {
        return "image/gif";
    } else if (filename.endsWith(".ico")) {
        return "image/x-icon";
    }

    return "application/octet-stream";
}

const String IoT_Server::getMethodName(const HTTPMethod method) {
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
        default:
            return "UNKNOWN_HTTP_METHOD";
    }
}

void IoT_Server::sendOptionsHeaders() {
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.sendHeader("Access-Control-Max-Age", "10000");
    webServer.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    webServer.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    webServer.send(200, "text/plain");
}

void IoT_Server::getControls() {
    DynamicJsonBuffer buffer(bufferSize);
    JsonObject& response = buffer.createObject();
    for (IoT_Control* control : controls) {
        control->serializeJsonTo(response);
    }
    sendResponse(response);
}

void IoT_Server::setControls() {
    DynamicJsonBuffer buffer(bufferSize);
    JsonObject& response = buffer.createObject();
    for (IoT_Control* control : controls) {
        if (webServer.hasArg(control->getId())) {
            control->setValue(webServer.arg(control->getId()).toInt());
            control->serializeJsonTo(response);
        }
    }
    sendResponse(response);
}
