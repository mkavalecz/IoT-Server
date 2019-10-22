#define EXAMPLE
#ifdef EXAMPLE

#define IOT_TITLE "IoT-Server"

#include "Arduino.h"
#include "IoT_Server.h"
#include "controls/IoT_Slider.h"
#include "controls/IoT_Button.h"
#include "controls/IoT_Checkbox.h"

void button1Pressed();
void increaseLED1();

bool autoIncrement = false;

IoT_Server server({new IoT_Slider(IOT_CONTROL_LED, "Control LED", D4, true, 1023), new IoT_Slider("led1", "LED 1", D1),
    new IoT_Slider("led2", "LED 2", D2), (new IoT_Button("button1", "Button 1", D3))->setOnPress(button1Pressed),
    (new IoT_Checkbox("autoIncrement", "Autoincrement LED 1", 0))->setOnChange([&](const bool value) {
        autoIncrement = value;
    })});

void setup() {
    server.setup();
}

void loop() {
    server.loop();
    increaseLED1();
}

void button1Pressed() {
    server.setValue("led2", server.getValue("led2") + 25);
}

void increaseLED1() {
    if (autoIncrement) {
        server.setValue("led1", server.getValue("led1") + 25);
    }
}

#endif