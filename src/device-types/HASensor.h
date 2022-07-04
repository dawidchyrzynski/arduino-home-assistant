#ifndef AHA_HASENSOR_H
#define AHA_HASENSOR_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_SENSOR

class HASensor : public HABaseDeviceType
{
public:
    /**
     * Initializes sensor.
     *
     * @param uniqueId Unique ID of the sensor. Recommended characters: [a-z0-9\-_]
     */
    HASensor(const char* uniqueId);

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/sensor/#device-class
     * 
     * @param class Class name
     */
    inline void setDeviceClass(const char* deviceClass)
        { _deviceClass = deviceClass; }

    /**
     * Sends update events even if the value hasn’t changed. Useful if you want to have meaningful value graphs in history.
     *
     * @param forceUpdate
     */
    inline void setForceUpdate(bool forceUpdate)
        { _forceUpdate = forceUpdate; }

    /**
     * Sets icon of the sensor.
     * Any icon from MaterialDesignIcons.com. Prefix name with mdi:, ie mdi:home.
     *
     * @param class Icon name
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Defines the units of measurement of the sensor, if any.
     *
     * @param units For example: °C, %
     */
    inline void setUnitOfMeasurement(const char* unitOfMeasurement)
        { _unitOfMeasurement = unitOfMeasurement; }

    /**
     * Defines a template to extract the value. ny.
     *
     * @param template
     */
    inline void setValueTemplate(const char* valueTemplate)
        { _valueTemplate = valueTemplate; }   

    /**
     * Publishes value of the sensor.
     * 
     * @param value
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setValue(const char* value);

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;

private:
    const char* _deviceClass;
    bool _forceUpdate;
    const char* _icon;
    const char* _unitOfMeasurement;
    const char* _valueTemplate;
};

#endif
#endif
