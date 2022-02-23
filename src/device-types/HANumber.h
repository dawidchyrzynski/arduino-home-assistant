#ifndef AHA_HANumber_H
#define AHA_HANumber_H

#include "BaseDeviceType.h"



#ifdef ARDUINOHA_NUMBER

#define HANumber_CALLBACK(name) void (*name)(float, HANumber*)


class HANumber : public BaseDeviceType
{
public:
    /**
     * Initializes binary sensor.
     *
     * @param uniqueId Unique ID of the sensor. Recommendes characters: [a-z0-9\-_]
     */
    HANumber(
        const char* uniqueId
    );
    HANumber(
        const char* uniqueId,
        HAMqtt& mqtt
    ); // legacy constructor

    /**
     * Publishes configuration of the sensor to the MQTT.
     */
    virtual void onMqttConnected() override;

        /**
     * Processes message received from the MQTT broker.
     * The method updates state of the switch (if message matches switch'es topic).
     */
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) override;

    /**
     * Publishes new value of the sensor.
     * Please note that connection to MQTT broker must be acquired.
     * Otherwise method will return false.
     *
     * @param state Value to publish.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setValue(float value, bool force = false);


    inline float getValue(){ return _value;}

    void setStep(float step){_step = step;}

    void setPrecision(uint8_t precision){_precision = precision;}

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

      /**
     * Registers callback that will be called each time the state of the switch changes.
     * Please note that it's not possible to register multiple callbacks for the same switch.
     *
     * @param callback
     */
    inline void onValueChanged(HANumber_CALLBACK(callback))
        { _valueCallback = callback; }

private:
    bool publishValue(float value);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;

    float _value;
    uint8_t _precision;
    const char* _units;
    const char* _icon;
    bool _retain;
    float _step;

    HANumber_CALLBACK(_valueCallback);

};

#endif
#endif
