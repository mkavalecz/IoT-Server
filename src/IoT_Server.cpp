#include "IoT_Server.h"

IoT_Server::IoT_Server(const std::initializer_list<IoT_Control*> controls)
    : webServer(80)
    , webSocket(8080)
    , controls(controls)
    , setupComplete(false)
    , bufferSize(JSON_OBJECT_SIZE(controls.size()) + (controls.size() * JSON_OBJECT_SIZE(10)) + 100) {

    for (IoT_Control* control : controls) {
        controlMap[control->getId()] = control;
    }

    controlLED = findControlLED();
}

void IoT_Server::setup() {
    Serial.begin(IOT_BAUD_RATE);
    SPIFFS.begin();

    enableControlLED();

    setupControls();
    setupWifi();
    setupAuthentication();
    setupWebServer();
    setupWebSocket();
    printUrl();

    disableControlLED();
    setupComplete = true;
}

void IoT_Server::loop() {
    for (IoT_Control* control : controls) {
        int status = control->loop();
        if (status != IOT_STATUS_UNCHANGED) {
            DynamicJsonDocument notification(bufferSize);
            sendNotification(notification);
        }
    }
    webSocket.loop();
    webServer.handleClient();
}

void IoT_Server::handleRequest(const char* uri, const HTTPMethod method, const std::function<void(void)> callback) {
    Debug::print("Handling request: ");
    Debug::print(uri);
    Debug::print(", ");
    Debug::println(getMethodName(method));

    webServer.on(uri, method, callback);
    if (method != HTTP_OPTIONS) {
        webServer.on(uri, HTTP_OPTIONS, [&]() { sendOptionsHeaders(); });
    }
}

bool IoT_Server::checkAuthentication() {
    if (!webServer.authenticate(userName.c_str(), password.c_str())) {
        webServer.requestAuthentication();
        return false;
    }
    return true;
}

void IoT_Server::sendNotification(const DynamicJsonDocument& notification) {
    String notificationString = getJsonString(notification);
    Debug::print("Notification: ");
    Debug::println(notificationString.c_str());
    webSocket.broadcastTXT(notificationString);
}

void IoT_Server::sendResponse(const DynamicJsonDocument& response) {
    String responseString = getJsonString(response);
    Debug::print("Response: ");
    Debug::println(responseString.c_str());
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", responseString);
}

const char* IoT_Server::getParameter(const char* id) {
    return webServer.arg(id).c_str();
}

const int IoT_Server::getValue(const char* id) {
    std::map<const char*, IoT_Control*>::iterator it = controlMap.find(id);
    if (it == controlMap.end()) {
        return 0;
    }

    return it->second->getValue();
}

const int IoT_Server::setValue(const char* id, const int value) {
    std::map<const char*, IoT_Control*>::iterator it = controlMap.find(id);
    if (it == controlMap.end()) {
        return 0;
    }

    int result = it->second->setValue(value);

    DynamicJsonDocument notification(bufferSize);
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

const String IoT_Server::getJsonString(const DynamicJsonDocument& json) {
    String jsonString = String();
    serializeJson(json, jsonString);

#ifdef IOT_DEBUG
    response.printTo(Serial);
    Serial.println("");
#endif

    return jsonString;
}

IoT_Slider* IoT_Server::findControlLED() {
    std::map<const char*, IoT_Control*>::iterator it = controlMap.find(IOT_CONTROL_LED);
    if (it != controlMap.end()) {
        return (IoT_Slider*) it->second;
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

void IoT_Server::setupControls() {
    Serial.println("Initializing controls:");
    for (IoT_Control* control : controls) {
        control->setup();

        Serial.print(control->getId());
        Serial.print(" (");
        Serial.print(control->getName());
        Serial.println(")");
    }
}

void IoT_Server::setupWifi() {
    if (!SPIFFS.exists(IOT_WIFI_CONFIG)) {
        Serial.print("WiFi config not found on file system. Please upload ");
        Serial.print(IOT_WIFI_CONFIG);
        Serial.println(" from data folder.");
    }
    String ssid[IOT_NUM_SSID];
    String password[IOT_NUM_SSID];

    Serial.println("Reading wifi config from file.");
    File file = SPIFFS.open(IOT_WIFI_CONFIG, "r");
    for (int i = 0; i < IOT_NUM_SSID; i++) {
        ssid[i] = file.readStringUntil(';');
        password[i] = file.readStringUntil(';');

        Debug::print("SSID ");
        Debug::print(i);
        Debug::print(": ");
        Debug::println(ssid[i]);
    }
    file.close();
    Debug::println("End of wifi config.");

    Serial.print("Waiting for one of the configured networks to show up.");
    bool found = false;
    while (true) {
        if (!found) {
            int n = WiFi.scanNetworks();
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < IOT_NUM_SSID; j++) {
                    if (ssid[j] == "") {
                        continue;
                    }
                    if (WiFi.SSID(i) == ssid[j]) {
                        WiFi.begin(ssid[j].c_str(), password[j].c_str());
                        Serial.println("");
                        Serial.print("Connecting to ");
                        Serial.print(ssid[j]);
                        found = true;
                        break;
                    }
                }
            }
        }
        if (WiFi.status() == WL_CONNECTED) {
            break;
        }
        delay(500);
        Serial.print(".");
        blinkControlLED();
    }
    enableControlLED();
    Serial.println("");
    Serial.println("WiFi connected.");
}

void IoT_Server::setupAuthentication() {
    if (!SPIFFS.exists(IOT_AUTH_CONFIG)) {
        Serial.print("Authentication config not found on file system. Please upload ");
        Serial.print(IOT_AUTH_CONFIG);
        Serial.println(" from data folder.");
        Serial.println("Using admin/admin as fallback user/password.");
        userName = "admin";
        password = "admin";
        return;
    }

    Serial.println("Reading authentication config from file.");
    File file = SPIFFS.open(IOT_AUTH_CONFIG, "r");
    userName = file.readStringUntil(';');
    password = file.readStringUntil(';');

    Debug::print("Username: ");
    Debug::println(userName);

    file.close();
    Debug::println("End of authentication config.");
}

void IoT_Server::setupWebServer() {
    Debug::println("Web server basic setup...");
    webServer.onNotFound([&]() { handleFiles(); });
    webServer.on("/get", HTTP_GET, [&]() { getControls(); });
    webServer.on("/get", HTTP_OPTIONS, [&]() { sendOptionsHeaders(); });
    webServer.on("/set", HTTP_POST, [&]() { setControls(); });
    webServer.on("/set", HTTP_OPTIONS, [&]() { sendOptionsHeaders(); });
    webServer.on("/save", HTTP_POST, [&]() { saveControls(); });
    webServer.on("/save", HTTP_OPTIONS, [&]() { sendOptionsHeaders(); });
    webServer.on("/title", HTTP_GET, [&]() { getTitle(); });
    webServer.on("/title", HTTP_OPTIONS, [&]() { sendOptionsHeaders(); });
    Debug::println("Starting web server.");
    webServer.begin();
}

void IoT_Server::setupWebSocket() {
    Debug::println("Starting web socket.");
    webSocket.begin();
}

void IoT_Server::printUrl() {
    Serial.println("Setup successful.");
    Serial.print("URL: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void IoT_Server::handleFiles() {
    if (!checkAuthentication()) {
        return;
    }
    String path;
    if (webServer.uri() == "/") {
        path = "/static/index.html";
    } else {
        path = "/static" + webServer.uri();
    }

    if (!SPIFFS.exists(path)) {
        webServer.send(404, "text/plain", "404: File Not Found");
        return;
    }

    File file = SPIFFS.open(path, "r");
    webServer.streamFile(file, getContentType(path));
    file.close();
}

const char* IoT_Server::getContentType(const String& filename) {
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

const char* IoT_Server::getMethodName(const HTTPMethod method) {
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

void IoT_Server::getTitle() {
    if (!checkAuthentication()) {
        return;
    }
    DynamicJsonDocument response(bufferSize);
    response["title"] = IOT_TITLE;
    sendResponse(response);
}

void IoT_Server::getControls() {
    if (!checkAuthentication()) {
        return;
    }
    DynamicJsonDocument response(bufferSize);
    for (IoT_Control* control : controls) {
        control->serializeJsonTo(response);
    }
    sendResponse(response);
}

void IoT_Server::setControls() {
    if (!checkAuthentication()) {
        return;
    }
    DynamicJsonDocument response(bufferSize);
    for (IoT_Control* control : controls) {
        if (webServer.hasArg(control->getId())) {
            control->setValue(webServer.arg(control->getId()).toInt());
            control->serializeJsonTo(response);
        }
    }
    sendResponse(response);
}

void IoT_Server::saveControls() {
    if (!checkAuthentication()) {
        return;
    }
    DynamicJsonDocument response(bufferSize);
    for (IoT_Control* control : controls) {
        if (webServer.hasArg(control->getId())) {
            response[control->getId()] = control->saveState();
        }
    }
    sendResponse(response);
}
