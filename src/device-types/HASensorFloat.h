#ifndef AHA_HASENSORFLOAT_H
#define AHA_HASENSORFLOAT_H

#include "HASensor.h"

#ifndef EX_ARDUINOHA_SENSOR

/**
 * HASensorFloat allows to publish floating point values of a sensor that will be displayed in the HA panel.
 * The functionality of this class is simple to keep resource usage as low as possible.
 * 
 * @note You can find more information about this class in HASensor documentation.
 */
class HASensorFloat : public HASensor
{
public:
    /// Represents precision of the sensor.
    enum Precision {
        /// No digits after the decimal point.
        PrecisionP0,

        /// One digit after the decimal point.
        PrecisionP1,
        
        /// Two digits after the decimal point.
        PrecisionP2,

        /// Three digits after the decimal point.
        PrecisionP3,

        /// Four digits after the decimal point.
        PrecisionP4
    };

    /**
     * @param uniqueId The unique ID of the sensor. It needs to be unique in a scope of your device.
     * @param precision Precision of the floating point number that will be displayed in the HA panel.
     */
    HASensorFloat(
        const char* uniqueId,
        const Precision precision = PrecisionP2
    );

    /**
     * Changes value of the sensor and publish MQTT message.
     * Please note that if a new value is the same as the previous one the MQTT message won't be published.
     *
     * @param value New value of the sensor.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool setValue(const float value, const bool force = false);

    /**
     * Sets the current value of the sensor without publishing it to Home Assistant.
     * This method may be useful if you want to change the value before the connection with the MQTT broker is acquired.
     *
     * @param value New value of the sensor.
     */
    inline void setCurrentValue(const float value)
        { _currentValue = value; }

    /**
     * Returns the last known value of the sensor.
     * By default it's zero.
     */
    inline float getCurrentValue() const
        { return _currentValue; }

protected:
    virtual void onMqttConnected() override;

private:
    /**
     * Publishes the MQTT message with the given value.
     * 
     * @param state The value to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishValue(const float value);

    /**
     * Sets the sensor's template that will be used by HA to extract the number.
     */
    void initValueTemplate();

    /**
     * Converts the given floating point value into integer that will be published to HA.
     * 
     * @param value Value to convert.
     */
    int32_t processValue(const float value) const;

    /// The precision of the sensor. By default it's `HASensorFloat::PrecisionP2`.
    Precision _precision;

    /// The current value of the sensor. By defaults it's zero.
    float _currentValue;
};

#endif
#endif