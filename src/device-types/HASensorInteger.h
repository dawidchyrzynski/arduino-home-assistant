#ifndef AHA_HASENSORINTEGER_H
#define AHA_HASENSORINTEGER_H

#include "HASensor.h"

#ifndef EX_ARDUINOHA_SENSOR

/**
 * HASensorInteger allows to publish integer values of a sensor that will be displayed in the HA panel.
 * The functionality of this class is simple to keep resource usage as low as possible.
 * 
 * @note You can find more information about this class in HASensor documentation.
 */
class HASensorInteger : public HASensor
{
public:
    /**
     * @param uniqueId The unique ID of the sensor. It needs to be unique in a scope of your device.
     */
    HASensorInteger(const char* uniqueId);

    /**
     * Changes value of the sensor and publish MQTT message.
     * Please note that if a new value is the same as the previous one the MQTT message won't be published.
     *
     * @param value New value of the sensor.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns true if the MQTT message has been published successfully.
     */
    bool setValue(const int32_t value, const bool force = false);

    /**
     * Sets the current value of the sensor without publishing it to Home Assistant.
     * This method may be useful if you want to change the value before the connection with the MQTT broker is acquired.
     *
     * @param value New value of the sensor.
     */
    inline void setCurrentValue(const int32_t value)
        { _currentValue = value; }

    /**
     * Returns the last known value of the sensor.
     * By default it's zero.
     */
    inline int32_t getCurrentValue() const
        { return _currentValue; }

protected:
    virtual void onMqttConnected() override;

private:
    /**
     * Publishes the MQTT message with the given value.
     * 
     * @param state The value to publish.
     * @returns Returns true if the MQTT message has been published successfully.
     */
    bool publishValue(const int32_t value);

    /// The current value of the sensor. by default it's zero.
    int32_t _currentValue;
};

#endif
#endif