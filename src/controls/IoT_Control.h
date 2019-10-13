#ifndef IOT_CONTROL_H
#define IOT_CONTROL_H

#define IOT_STATUS_UNCHANGED -1

class IoT_Control {
  protected:
    const String id;
    const String name;
    const int pin;
    int value;

  public:
    IoT_Control(const String id, const String name, const int pin, const int value)
        : id(id)
        , name(name)
        , pin(pin)
        , value(value) {
    }

    virtual void setup() = 0;
    virtual const int loop() = 0;
    virtual const String getTypeName() const = 0;

    virtual void serializeJsonTo(DynamicJsonDocument& object) const {
        object[getId()]["name"] = getName();
        object[getId()]["type"] = getTypeName();
        object[getId()]["value"] = getValue();
    }

    virtual const String getId() const {
        return id;
    }

    virtual const String getName() const {
        return name;
    }

    virtual const int setValue(int value) {
        this->value = value;

#ifdef IoT_TRACE
        Serial.println(getName() + "=" + this->value);
#endif

        return value;
    }

    virtual const int getValue() const {
        return value;
    }

    virtual ~IoT_Control() {
    }
};

#endif // IOT_CONTROL_H