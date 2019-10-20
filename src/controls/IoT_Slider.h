#ifndef IOT_SLIDER_H
#define IOT_SLIDER_H

#include "IoT_Control.h"

class IoT_Slider : public IoT_Control {
  private:
    const bool inverted;
    const int minValue;
    const int maxValue;

  protected:
    std::function<void(int)> onChange;

  public:
    IoT_Slider(const String id, const String name)
        : IoT_Slider(id, name, IOT_NOT_CONNECTED, false, 0, 0, 255, false) {
    }

    IoT_Slider(const String id, const String name, const bool inverted, const int initialValue, const int minValue,
        const int maxValue)
        : IoT_Slider(id, name, IOT_NOT_CONNECTED, inverted, initialValue, minValue, maxValue, false) {
    }

    IoT_Slider(const String id, const String name, const int initialValue, const int minValue, const int maxValue,
        const bool showOnSettings)
        : IoT_Slider(id, name, IOT_NOT_CONNECTED, false, initialValue, minValue, maxValue, showOnSettings) {
    }

    IoT_Slider(const String id, const String name, const bool inverted, const int initialValue, const int minValue,
        const int maxValue, const bool showOnSettings)
        : IoT_Slider(id, name, IOT_NOT_CONNECTED, inverted, initialValue, minValue, maxValue, showOnSettings) {
    }

    IoT_Slider(const String id, const String name, const int pin)
        : IoT_Slider(id, name, pin, false, 0, 0, 255, false) {
    }

    IoT_Slider(const String id, const String name, const int pin, const bool showOnSettings)
        : IoT_Slider(id, name, pin, false, 0, 0, 255, showOnSettings) {
    }

    IoT_Slider(const String id, const String name, const int pin, const bool inverted, const int maxValue)
        : IoT_Slider(id, name, pin, inverted, 0, 0, maxValue, false) {
    }

    IoT_Slider(const String id, const String name, const int pin, const bool inverted, const int maxValue,
        const bool showOnSettings)
        : IoT_Slider(id, name, pin, inverted, 0, 0, maxValue, showOnSettings) {
    }

    IoT_Slider(const String id, const String name, const int pin, const bool inverted, const int initialValue,
        const int minValue, const int maxValue, const bool showOnSettings)
        : IoT_Control(id, name, pin, showOnSettings, initialValue)
        , inverted(inverted)
        , minValue(minValue)
        , maxValue(maxValue) {
    }

    IoT_Slider* setOnChange(const std::function<void(int)> onChange) {
        this->onChange = onChange;
        return this;
    }

    virtual void setup() {
        if (pin == IOT_NOT_CONNECTED) {
            return;
        }
        pinMode(pin, OUTPUT);
    }

    virtual const int loop() {
        if (pin == IOT_NOT_CONNECTED) {
            return IOT_STATUS_UNCHANGED;
        }

        if (inverted) {
            analogWrite(pin, maxValue - value);
        } else {
            analogWrite(pin, value);
        }
        return IOT_STATUS_UNCHANGED;
    }

    virtual const int setValue(const int value) {
        if (onChange) {
            onChange(value);
        }
        if (value < minValue) {
            return IoT_Control::setValue(minValue);
        }
        return IoT_Control::setValue(value % (maxValue + 1));
    }

    virtual void serializeJsonTo(DynamicJsonDocument& object) const {
        IoT_Control::serializeJsonTo(object);
        object[getId()]["minValue"] = minValue;
        object[getId()]["maxValue"] = maxValue;
    }

    void enable() {
        setValue(maxValue);
    }

    void disable() {
        setValue(0);
    }

    void blink() {
        if (getValue() == maxValue) {
            setValue(0);
        } else {
            setValue(maxValue);
        }
    }

    virtual const String getTypeName() const {
        return "slider";
    }

    virtual ~IoT_Slider() {
    }
};

#endif // IOT_SLIDER_H