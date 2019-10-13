#ifndef IOT_LED_H
#define IOT_LED_H

#include "IoT_Control.h"

class IoT_LED : public IoT_Control {
  private:
    const bool inverted;
    const unsigned int maxBrightness;

  public:
    IoT_LED(const String id, const String name, const unsigned int pin)
        : IoT_LED(id, name, pin, false, 255) {
    }

    IoT_LED(const String id, const String name, const unsigned int pin, const bool inverted)
        : IoT_LED(id, name, pin, inverted, 255) {
    }

    IoT_LED(const String id, const String name, const unsigned int pin, const bool inverted,
        const unsigned int maxBrightness)
        : IoT_Control(id, name, pin, 0)
        , inverted(inverted)
        , maxBrightness(maxBrightness) {
    }

    virtual void setup() {
        pinMode(pin, OUTPUT);
    }

    virtual const int loop() {
        if (inverted) {
            analogWrite(pin, maxBrightness - value);
        } else {
            analogWrite(pin, value);
        }
        return IOT_STATUS_UNCHANGED;
    }

    virtual const String getTypeName() const {
        return "led";
    }

    virtual void serializeJsonTo(DynamicJsonDocument& object) const {
        IoT_Control::serializeJsonTo(object);
        object[getId()]["maxBrightness"] = maxBrightness;
    }

    virtual const unsigned int setValue(const unsigned int value) {
        return IoT_Control::setValue(value % (maxBrightness + 1));
    }

    void enable() {
        setValue(maxBrightness);
    }

    void disable() {
        setValue(0);
    }

    void blink() {
        if (getValue() == maxBrightness) {
            setValue(0);
        } else {
            setValue(maxBrightness);
        }
    }

    virtual ~IoT_LED() {
    }
};

#endif // IOT_LED_H