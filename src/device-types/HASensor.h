#ifndef AHA_HASENSOR_H
#define AHA_HASENSOR_H

#include "BaseDeviceType.h"
#include "../HAUtils.h"

#ifdef ARDUINOHA_SENSOR

template <typename T>
class HASensor : public BaseDeviceType
{
public:
    /**
     * Initializes binary sensor.
     *
     * @param name Name of the sensor. Recommendes characters: [a-z0-9\-_]
     * @param initialValue Initial value of the sensor.
                           It will be published right after "config" message in order to update HA state.
     */
    HASensor(
        const char* name,
        T initialValue
    );
    HASensor(
        const char* name,
        T initialValue,
        HAMqtt& mqtt
    ); // legacy constructor

    /**
     * Initializes binary sensor with the specified class.
     * You can find list of available values here: https://www.home-assistant.io/integrations/binary_sensor/#device-class
     *
     * @param name Name of the sensor. Recommendes characters: [a-z0-9\-_]
     * @param deviceClass Name of the class (lower case).
     * @param initialValue Initial value of the sensor.
                           It will be published right after "config" message in order to update HA state.
     */
    HASensor(
        const char* name,
        const char* deviceClass,
        T initialValue
    );
    HASensor(
        const char* name,
        const char* deviceClass,
        T initialValue,
        HAMqtt& mqtt
    ); // legacy constructor

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
    bool setValue(T value);

    /**
     * Returns last known state of the sensor.
     * If setState method wasn't called the initial value will be returned.
     */
    inline T getValue() const
        { return _currentValue; }

    /**
     * Defines the units of measurement of the sensor, if any.
     *
     * @param units For example: °C, %
     */
    inline void setUnitOfMeasurement(const char* units)
        { _units = units; }

    /**
     * Sets icon of the sensor, e.g. `mdi:home`.
     *
     * @param icon Material Design Icon name with mdi: prefix.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

private:
    void publishConfig();
    bool publishValue(T value);
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    bool writeSerializedData(const char* serializedDevice) const;
    uint16_t calculateValueLength() const;
    bool valueToStr(char* dst, T value) const;

    const char* _class;
    const char* _units;
    HAUtils::ValueType _valueType;
    T _currentValue;
    const char* _icon;
};

#endif
#endif
