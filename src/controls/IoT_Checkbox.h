#ifndef IOT_CHECKBOX_H
#define IOT_CHECKBOX_H

#include "IoT_Control.h"

class IoT_Checkbox : public IoT_Control {
  protected:
    std::function<void(bool)> onChange;

  public:
    IoT_Checkbox(const char* id, const char* name, const int initialValue)
        : IoT_Checkbox(id, name, IOT_NOT_CONNECTED, false, initialValue) {
    }

    IoT_Checkbox(const char* id, const char* name, const int initialValue, const bool showOnSettings)
        : IoT_Checkbox(id, name, IOT_NOT_CONNECTED, showOnSettings, initialValue) {
    }

    IoT_Checkbox(const char* id, const char* name, const int pin, const int initialValue, const bool showOnSettings)
        : IoT_Control(id, name, pin, showOnSettings, initialValue) {
    }

    IoT_Checkbox* setOnChange(const std::function<void(bool)> onChange) {
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

        digitalWrite(pin, (value != 0) ? HIGH : LOW);

        return IOT_STATUS_UNCHANGED;
    }

    virtual const int setValue(const int value) {
        onChange(value != 0);
        return IoT_Control::setValue(value != 0 ? 1 : value);
    }

    virtual const char* getTypeName() const {
        return "checkbox";
    }

    virtual ~IoT_Checkbox() {
    }
};

#endif // IOT_CHECKBOX_H