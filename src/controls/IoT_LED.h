#ifndef IOT_LED_H
#define IOT_LED_H

#include "IoT_Control.h"

class IoT_LED : public IoT_Control {
  private:
    const bool inverted;
    const unsigned int maxBrightness;

  public:
    IoT_LED(const String name, const unsigned int pin)
        : IoT_LED(name, pin, false, 255) {
    }

    IoT_LED(const String name, const unsigned int pin, const bool inverted)
        : IoT_LED(name, pin, inverted, 255) {
    }

    IoT_LED(const String name, const unsigned int pin, const bool inverted, const unsigned int maxBrightness)
        : IoT_Control(name, pin, 0)
        , inverted(inverted)
        , maxBrightness(maxBrightness) {
    }

    virtual void setup() {
        pinMode(pin, OUTPUT);
    }

    virtual const int loop() {
        analogWrite(pin, value);
        return IOT_STATUS_UNCHANGED;
    }

    virtual const unsigned int setValue(const unsigned int value) {
        if (inverted) {
            return IoT_Control::setValue(maxBrightness - (value % (maxBrightness + 1)));
        } else {
            return IoT_Control::setValue(value % maxBrightness);
        }
    }

    void enable() {
        setValue(maxBrightness);
    }

    void disable() {
        setValue(0);
    }

    void blink() {
        if (getValue() == maxBrightness) {
            if (inverted) {
                setValue(maxBrightness);
            } else {
                setValue(0);
            }
        } else {
            if (inverted) {
                setValue(0);
            } else {
                setValue(maxBrightness);
            }
        }
    }

    virtual ~IoT_LED() {
    }
};

#endif // IOT_LED_H