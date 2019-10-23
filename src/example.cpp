#define IOT_TITLE "IoT LED Pumpkin"

#include "IoT_Server.h"
#include "controls/IoT_Button.h"
#include "controls/IoT_Checkbox.h"
#include "controls/IoT_Slider.h"

bool flicker = false;

int counter = 0;
int counterMax = 2000;

int redMin = 50;
int redMax = 100;

int orangeMin = 80;
int orangeMax = 255;

int yellowMin = 40;
int yellowMax = 120;

IoT_Server server({new IoT_Slider(IOT_CONTROL_LED, "Control LED", D4, true, 1023), new IoT_Slider("red", "Red LED", D8),
    new IoT_Slider("orange", "Orange LED", D7), new IoT_Slider("yellow", "Yellow LED", D6),
    (new IoT_Checkbox("flicker", "Flicker", 0))->setOnChange([&](const bool value) { flicker = value; }),
    (new IoT_Slider("delayCounter", "Delay Counter", counterMax, 1000, 10000, true))->setOnChange([&](const int value) {
        counterMax = value;
    }),
    (new IoT_Slider("redMin", "Red LED Minimum flicker", redMin, 0, 255, true))->setOnChange([&](const int value) {
        redMin = value;
    }),
    (new IoT_Slider("redMax", "Red LED Maximum flicker", redMax, 0, 255, true))->setOnChange([&](const int value) {
        redMax = value;
    }),
    (new IoT_Slider("orangeMin", "Orange LED Minimum flicker", orangeMin, 0, 255, true))
        ->setOnChange([&](const int value) { orangeMin = value; }),
    (new IoT_Slider("orangeMax", "Orange LED Maximum flicker", orangeMax, 0, 255, true))
        ->setOnChange([&](const int value) { orangeMax = value; }),
    (new IoT_Slider("yellowMin", "Yellow LED Minimum flicker", yellowMin, 0, 255, true))
        ->setOnChange([&](const int value) { yellowMin = value; }),
    (new IoT_Slider("yellowMax", "Yellow LED Maximum flicker", yellowMax, 0, 255, true))
        ->setOnChange([&](const int value) { yellowMax = value; })});

void setup() {
    server.setup();
}

void loop() {
    server.loop();
    if (flicker) {
        if (counter < counterMax) {
            counter++;
        } else {
            server.setValue("red", random(redMin, redMax));
            server.setValue("orange", random(orangeMin, orangeMax));
            server.setValue("yellow", random(yellowMin, yellowMax));
            counter = 0;
        }
    }
}