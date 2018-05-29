#ifndef IOT_BUTTON_H
#define IOT_BUTTON_H

#include "IoT_Control.h"

class IoT_Button : public IoT_Control {
  protected:
    int pressedState;
    unsigned int debounceDelay;

    unsigned long lastDebounceTime;
    unsigned int oldValue;

  public:
    std::function<void()> onPress;
    std::function<void(bool)> onChange;

    IoT_Button(const String name, const unsigned int pin)
        : IoT_Control(name, pin, 0)
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

    virtual void setup() {
        pinMode(pin, INPUT);
    }

    virtual const int loop() {
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

    virtual const unsigned int setValue(const unsigned int value) {
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