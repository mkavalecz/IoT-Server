#ifndef IOT_CONTROL_H
#define IOT_CONTROL_H

#define IOT_STATUS_UNCHANGED -1

class IoT_Control {
  protected:
    const String name;
    const unsigned int pin;
    unsigned int value;

  public:
    IoT_Control(const String name, const unsigned int pin, const unsigned int value)
        : name(name)
        , pin(pin)
        , value(value) {
    }

    virtual void setup() = 0;
    virtual const int loop() = 0;

    virtual const String getName() const {
        return name;
    }

    virtual const unsigned int setValue(unsigned int value) {
        this->value = value;

#ifdef IoT_TRACE
        Serial.println(getName() + "=" + this->value);
#endif

        return value;
    }

    virtual const unsigned int getValue() const {
        return value;
    }

    virtual ~IoT_Control() {
    }
};

#endif // IOT_CONTROL_H