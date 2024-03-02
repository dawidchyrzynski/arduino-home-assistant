#ifndef AHA_HASENSOR_H
#define AHA_HASENSOR_H

#include "HABaseDeviceType.h"
#include "../utils/HANumeric.h"

#ifndef EX_ARDUINOHA_SENSOR

/**
 * HASensor allows to publish textual sensor values that will be displayed in the HA panel.
 * If you need to publish numbers then HASensorNumber is what you're looking for.
 *
 * @note It's not possible to define a sensor that publishes mixed values (e.g. string + integer values).
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/sensor.mqtt/
 */
class HASensor : public HABaseDeviceType
{
public:
    enum Features {
        DefaultFeatures = 0,
        JsonAttributesFeature = 1
    };

    /**
     * @param uniqueId The unique ID of the sensor. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the sensor.
     */
    HASensor(const char* uniqueId, const uint16_t features = DefaultFeatures);

    /**
     * Publishes the MQTT message with the given value.
     * Unlike the other device types, the HASensor doesn't store the previous value that was set.
     * It means that the MQTT message is produced each time the setValue method is called.
     *
     * @param value String representation of the sensor's value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setValue(const char* value);

    /**
     * Publishes the given JSON data on the JSON attributes topic.
     * The `JsonAttributesFeature` has to be enabled prior to setting the value.
     *
     * @param json JSON attributes.
     */
    bool setJsonAttributes(const char* json);

    /**
     * Sets the number of seconds after the sensor’s state expires, if it’s not updated.
     * By default the sensors state never expires.
     *
     * @param expireAfter The number of seconds.
     */
    void setExpireAfter(uint16_t expireAfter);

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/sensor/#device-class
     *
     * @param deviceClass The class name.
     */
    inline void setDeviceClass(const char* deviceClass)
        { _deviceClass = deviceClass; }

    /**
     * Sets the entity category for the sensor.
     * See: https://www.home-assistant.io/integrations/sensor.mqtt/#entity_category
     *
     * @param entityCategory The category name.
     */
    inline void setEntityCategory(const char* entityCategory)
        { _entityCategory = entityCategory; }

    /**
     * Sets class of the state for the long term stats.
     * See: https://developers.home-assistant.io/docs/core/entity/sensor/#long-term-statistics
     *
     * @param stateClass The class name.
     */
    inline void setStateClass(const char* stateClass)
        { _stateClass = stateClass; }

    /**
     * Forces HA panel to process each incoming value (MQTT message).
     * It's useful if you want to have meaningful value graphs in history.
     *
     * @param forceUpdate
     */
    inline void setForceUpdate(bool forceUpdate)
        { _forceUpdate = forceUpdate; }

    /**
     * Sets icon of the sensor.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param class The icon name.
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

protected:
    virtual void buildSerializer() override final;
    virtual void onMqttConnected() override;

private:
    /// Features enabled for the sensor.
    const uint16_t _features;

    /// The device class. It can be nullptr.
    const char* _deviceClass;

    /// The entity category for the sensor. It can be nullptr. See: https://www.home-assistant.io/integrations/sensor.mqtt/#entity_category
    const char* _entityCategory;

    /// The state class for the long term stats. It can be nullptr. See: https://developers.home-assistant.io/docs/core/entity/sensor/#long-term-statistics
    const char* _stateClass;

    /// The force update flag for the HA panel.
    bool _forceUpdate;

    /// The icon of the sensor. It can be nullptr.
    const char* _icon;

    /// The unit of measurement for the sensor. It can be nullptr.
    const char* _unitOfMeasurement;

    /// It defines the number of seconds after the sensor’s state expires, if it’s not updated. By default the sensors state never expires.
    HANumeric _expireAfter;
};

#endif
#endif
