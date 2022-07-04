#ifndef AHA_HABINARYSENSOR_H
#define AHA_HABINARYSENSOR_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_BINARY_SENSOR

class HABinarySensor : public HABaseDeviceType
{
public:
    HABinarySensor(const char* uniqueId);

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/binary_sensor/#device-class
     * 
     * @param class Class name
     */
    inline void setDeviceClass(const char* deviceClass)
        { _class = deviceClass; }

    /**
     * Sets icon of the sensor.
     * Any icon from MaterialDesignIcons.com. Prefix name with mdi:, ie mdi:home.
     *
     * @param class Icon name
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Changes state of the sensor and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the sensor.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(const bool state, const bool force = false);

    /**
     * Sets current state of the sensor without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the sensor.
     */
    inline void setCurrentState(const bool state)
        { _currentState = state; }

    /**
     * Returns last known state of the sensor.
     * If setState method wasn't called the initial value will be returned.
     */
    inline bool getCurrentState() const
        { return _currentState; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;

private:
    bool publishState(bool state);

    const char* _class;
    const char* _icon;
    bool _currentState;
};

#endif
#endif
