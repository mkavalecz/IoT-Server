#define IOT_TITLE "IoT LED Pumpkin"

#include "IoT_Server.h"
#include "controls/IoT_Button.h"
#include "controls/IoT_Checkbox.h"
#include "controls/IoT_Slider.h"

bool flicker = false;

int counter = 0;
int counterMax = 2000;

int redMin = 0;
int redMax = 1023;

int greenMin = 0;
int greenMax = 1023;

int yellowMin = 0;
int yellowMax = 1023;

IoT_Server server("IoT LED Pumpkin",
    {new IoT_Slider(IOT_CONTROL_LED, "Control LED", D4, true, true), new IoT_Slider("red", "Eyes - Red", D6),
        new IoT_Slider("green", "Nose - Green", D8), new IoT_Slider("yellow", "Mouth - Yellow", D7),
        (new IoT_Checkbox("flicker", "Flicker", 0))->setOnChange([&](const bool value) {
            flicker = value;
            if (!flicker) {
                server.setValue("red", 0);
                server.setValue("green", 0);
                server.setValue("yellow", 0);
            }
        }),
        (new IoT_Slider("delayCounter", "Delay Counter", IOT_NOT_CONNECTED, counterMax, 1000, 10000, true))
            ->setOnChange([&](const int value) { counterMax = value; }),
        (new IoT_Slider("redMin", "Red Minimum flicker", IOT_NOT_CONNECTED, redMin, true))
            ->setOnChange([&](const int value) { redMin = value; }),
        (new IoT_Slider("redMax", "Red Maximum flicker", IOT_NOT_CONNECTED, redMax, true))
            ->setOnChange([&](const int value) { redMax = value; }),
        (new IoT_Slider("greenMin", "Green Minimum flicker", IOT_NOT_CONNECTED, greenMin, true))
            ->setOnChange([&](const int value) { greenMin = value; }),
        (new IoT_Slider("greenMax", "Green Maximum flicker", IOT_NOT_CONNECTED, greenMax, true))
            ->setOnChange([&](const int value) { greenMax = value; }),
        (new IoT_Slider("yellowMin", "Yellow Minimum flicker", IOT_NOT_CONNECTED, yellowMin, true))
            ->setOnChange([&](const int value) { yellowMin = value; }),
        (new IoT_Slider("yellowMax", "Yellow Maximum flicker", IOT_NOT_CONNECTED, yellowMax, true))
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
            server.setValue("green", random(greenMin, greenMax));
            server.setValue("yellow", random(yellowMin, yellowMax));
            counter = 0;
        }
    }
}