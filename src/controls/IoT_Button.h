#ifndef IOT_BUTTON_H
#define IOT_BUTTON_H

#include "IoT_Control.h"

class IoT_Button : public IoT_Control {
  protected:
    int pressedState;
    unsigned int debounceDelay;

    unsigned long lastDebounceTime;
    int oldValue;

    std::function<void()> onPress;
    std::function<void(bool)> onChange;

  public:
    IoT_Button(const String id, const String name, const int pin)
        : IoT_Button(id, name, pin, false) {
    }

    IoT_Button(const String id, const String name, const int pin, const bool showOnSettings)
        : IoT_Control(id, name, pin, showOnSettings, 0)
        , pressedState(LOW)
        , debounceDelay(50)
        , lastDebounceTime(0)
        , oldValue(0) {
    }

    IoT_Button* setPressedState(const int pressedState) {
        this->pressedState = pressedState;
        return this;
    }

    IoT_Button* setDebounceDelay(const unsigned int debounceDelay) {
        this->debounceDelay = debounceDelay;
        return this;
    }

    IoT_Button* setOnPress(const std::function<void()> onPress) {
        this->onPress = onPress;
        return this;
    }

    IoT_Button* setOnChange(const std::function<void(bool)> onChange) {
        this->onChange = onChange;
        return this;
    }

    void manualPress() {
        this->onPress();
    }

    virtual void setup() {
        if (pin == IOT_NOT_CONNECTED) {
            return;
        }
        pinMode(pin, INPUT);
    }

    virtual const int loop() {
        if (pin == IOT_NOT_CONNECTED) {
            return IOT_STATUS_UNCHANGED;
        }
        bool measured = (digitalRead(pin) == pressedState);

        if (measured != oldValue) {
            lastDebounceTime = millis();
        }

        oldValue = measured;

        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (measured != value) {
                value = measured;

                if (onChange) {
                    onChange(value);
                }

                if (value) {
                    if (onPress) {
                        onPress();
                    }
                }

                return value;
            }
        }

        return IOT_STATUS_UNCHANGED;
    }

    virtual const String getTypeName() const {
        return "button";
    }

    virtual const int setValue(const int value) {
        if (value) {
            if (onPress) {
                onPress();
            }
        }
        return value;
    }

    virtual ~IoT_Button() {
    }
};

#endif // IOT_BUTTON_H