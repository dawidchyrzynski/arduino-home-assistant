#ifndef AHA_HABINARYSENSOR_H
#define AHA_HABINARYSENSOR_H

#include "BaseDeviceType.h"

class HABinarySensor : public BaseDeviceType
{
public:
    /**
     * Initializes binary sensor.
     *
     * @param name Name of the sensor. Recommendes characters: [a-z0-9\-_]
     * @param initialState Initial state of the sensor.
                           It will be published right after "config" message in order to update HA state.
     */
    HABinarySensor(
        const char* name,
        bool initialState,
        HAMqtt& mqtt
    );

    /**
     * Initializes binary sensor with the specified class.
     * You can find list of available values here: https://www.home-assistant.io/integrations/binary_sensor/#device-class
     *
     * @param name Name of the sensor. Recommendes characters: [a-z0-9\-_]
     * @param deviceClass Name of the class (lower case).
     * @param initialState Initial state of the sensor.
                           It will be published right after "config" message in order to update HA state.
     */
    HABinarySensor(
        const char* name,
        const char* deviceClass,
        bool initialState,
        HAMqtt& mqtt
    );

    /**
     * Publishes configuration of the sensor to the MQTT.
     */
    virtual void onMqttConnected() override;

    /**
     * Changes state of the sensor and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the sensor.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(bool state);

    /**
     * Returns last known state of the sensor.
     * If setState method wasn't called the initial value will be returned.
     */
    inline bool getState() const
        { return _currentState; }

private:
    void publishConfig();
    bool publishState(bool state);
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    bool writeSerializedData(const char* serializedDevice) const;

    const char* _class;
    bool _currentState;
};

#endif
