#ifndef IOT_SLIDER_H
#define IOT_SLIDER_H

#include "IoT_Control.h"

class IoT_Slider : public IoT_Control {
  private:
    const int minValue;
    const int maxValue;

  protected:
    std::function<void(int)> onChange;

  public:
    IoT_Slider(const String id, const String name, const int initialValue, const int minValue, const int maxValue)
        : IoT_Control(id, name, -1, initialValue)
        , minValue(minValue)
        , maxValue(maxValue) {
    }

    IoT_Slider* setOnChange(const std::function<void(int)> onChange) {
        this->onChange = onChange;
        return this;
    }

    virtual void setup() {
    }

    virtual const int loop() {
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

    virtual const String getTypeName() const {
        return "slider";
    }

    virtual ~IoT_Slider() {
    }
};

#endif // IOT_SLIDER_H