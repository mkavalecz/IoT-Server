#include "Arduino.h"
#include "IoT_Server.h"
#include "controls/IoT_LED.h"
#include "controls/IoT_Button.h"

void button1Pressed();

IoT_Server server({new IoT_LED(IoT_Control_LED, "Control LED", D4, true, 1023), new IoT_LED("led1", "LED 1", D1),
    new IoT_LED("led2", "LED 2", D2), (new IoT_Button("button1", "Button 1", D3))->setOnPress(button1Pressed)});

void setup() {
    server.setup("IoT_Wifi", "107_53cur3p455w0rd");
}

void loop() {
    server.loop();
}

void button1Pressed() {
    server.setValue("led2", server.getValue("led2") + 25);
}
