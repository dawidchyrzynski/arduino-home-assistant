#ifndef AHA_HASENSOR_H
#define AHA_HASENSOR_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_SENSOR

class HASensor : public BaseDeviceType
{
public:
    /**
     * Initializes binary sensor.
     *
     * @param name Name of the sensor. Recommendes characters: [a-z0-9\-_]
     */
    HASensor(
        const char* name
    );
    HASensor(
        const char* name,
        HAMqtt& mqtt
    ); // legacy constructor

    /**
     * Publishes configuration of the sensor to the MQTT.
     */
    virtual void onMqttConnected() override;

    /**
     * Publishes new value of the sensor.
     * Please note that connection to MQTT broker must be acquired.
     * Otherwise method will return false.
     *
     * @param state Value to publish.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setValue(const char* value);
    bool setValue(uint32_t value);
    bool setValue(int32_t value);
    bool setValue(double value, uint8_t precision = 2);
    bool setValue(float value, uint8_t precision = 2);

    /**
     * The type/class of the sensor to set the icon in the frontend.
     *
     * @param className https://www.home-assistant.io/integrations/sensor/#device-class
     */
    inline void setDeviceClass(const char* className)
        { _class = className; }

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
    bool publishValue(const char* value);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;

    const char* _class;
    const char* _units;
    const char* _icon;
};

#endif
#endif
