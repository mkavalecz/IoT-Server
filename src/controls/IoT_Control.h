#ifndef IOT_CONTROL_H
#define IOT_CONTROL_H

#include "Arduino.h"
#include "debug.h"

#define IOT_NOT_CONNECTED -1
#define IOT_STATUS_UNCHANGED -1

class IoT_Control {
  protected:
    String id;
    const char* name;
    const int pin;
    const bool showOnSettings;
    int oldValue;
    int value;

  public:
    IoT_Control(String id, const char* name, const int pin, const bool showOnSettings, const int value)
        : id(id)
        , name(name)
        , pin(pin)
        , showOnSettings(showOnSettings)
        , oldValue(value)
        , value(value) {
    }

    virtual void setup() = 0;
    virtual const int loop() = 0;
    virtual const char* getTypeName() const = 0;

    virtual void serializeJsonTo(DynamicJsonDocument& object) const {
        object[getId()]["name"] = getName();
        object[getId()]["type"] = getTypeName();
        object[getId()]["value"] = getValue();
        object[getId()]["showOnSettings"] = getShowOnSettings();
    }

    virtual String getId() const {
        return id;
    }

    virtual const char* getName() const {
        return name;
    }

    virtual const bool getShowOnSettings() const {
        return showOnSettings;
    }

    virtual const int setValue(int value) {
        this->value = value;

#ifdef IOT_TRACE
        Serial.println(getName() + "=" + this->value);
#endif

        return value;
    }

    virtual const int getValue() const {
        return value;
    }

    virtual const int getOldValue() const {
        return oldValue;
    }

    virtual ~IoT_Control() {
    }
};

#endif // IOT_CONTROL_H