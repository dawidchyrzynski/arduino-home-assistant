#ifndef AHA_HASENSORNUMBER_H
#define AHA_HASENSORNUMBER_H

#include "HASensor.h"
#include "../utils/HAUtils.h"

#ifndef EX_ARDUINOHA_SENSOR

/**
 * HASensorInteger allows to publish numeric values of a sensor that will be displayed in the HA panel.
 *
 * @note You can find more information about this class in HASensor documentation.
 */
class HASensorNumber : public HASensor
{
public:
    /**
     * @param uniqueId The unique ID of the sensor. It needs to be unique in a scope of your device.
     * @param precision Precision of the floating point number that will be displayed in the HA panel.
     */
    HASensorNumber(
        const char* uniqueId,
        const NumberPrecision precision = PrecisionP0
    );

    /**
     * Changes value of the sensor and publish MQTT message.
     * Please note that if a new value is the same as the previous one the MQTT message won't be published.
     *
     * @param value New value of the sensor.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool setValue(const HAUtils::Number value, const bool force = false);

    /**
     * Changes value of the sensor using the float value.
     * This method converts the given float into underlying data type. 
     *
     * @param value New value of the sensor.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    inline bool setValueFloat(const float value, const bool force = false)
        { return setValue(HAUtils::processFloatValue(value, _precision), force); }

    /**
     * Sets the current value of the sensor without publishing it to Home Assistant.
     * This method may be useful if you want to change the value before the connection with the MQTT broker is acquired.
     *
     * @param value New value of the sensor.
     */
    inline void setCurrentValue(const HAUtils::Number value)
        { _currentValue = value; }

    /**
     * Sets the current value of the sensor using the float type.
     *
     * @param value The float value to set. It will be converted into underlying data type.
     */
    inline void setCurrentValueFloat(const float value)
        { _currentValue = HAUtils::processFloatValue(value, _precision); }

    /**
     * Returns the last known value of the sensor.
     * By default it's `HAUtils::NumberMax`.
     */
    inline HAUtils::Number getCurrentValue() const
        { return _currentValue; }

    /**
     * Returns the last known value of the sensor as a float.
     * By default it's `HAUtils::FloatMax`.
     */
    inline float getCurrentValueFloat() const
        { return HAUtils::getFloatValue(_currentValue, _precision); }

protected:
    virtual void onMqttConnected() override;

private:
    /**
     * Publishes the MQTT message with the given value.
     *
     * @param state The value to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishValue(const HAUtils::Number value);

    /// The precision of the sensor. By default it's `HASensorNumber::PrecisionP0`.
    const NumberPrecision _precision;

    /// The current value of the sensor. By default it's `HAUtils::NumberMax`.
    HAUtils::Number _currentValue;
};

#endif
#endif