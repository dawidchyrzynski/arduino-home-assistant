#ifndef AHA_HAHVAC_H
#define AHA_HAHVAC_H

#include "HABaseDeviceType.h"
#include "../utils/HAUtils.h"

#ifndef EX_ARDUINOHA_HVAC

/**
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/climate.mqtt/
 */
class HAHVAC : public HABaseDeviceType
{
public:
    enum Features {
        DefaultFeatures = 0
    };

    /**
     * @param uniqueId The unique ID of the HVAC. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the HVAC.
     * @param precision The precision of temperatures reported by the HVAC.
     */
    HAHVAC(
        const char* uniqueId,
        const uint8_t features = DefaultFeatures,
        const NumberPrecision precision = PrecisionP1
    );

    /**
     * Changes current temperature of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param temperature New current temperature.
     * @param force Forces to update the temperature without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setCurrentTemperature(const float temperature, const bool force = false);

    /**
     * Sets current temperature of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to change temperature before connection
     * with MQTT broker is acquired.
     *
     * @param temperature New current temperature.
     */
    inline void setCurrentCurrentTemperature(const float temperature)
        { _currentTemperature = HAUtils::processFloatValue(temperature, _precision); }

    /**
     * Returns last known current temperature of the HVAC.
     * If setCurrentTemperature method wasn't called the initial value will be returned.
     */
    inline float getCurrentTemperature() const
        { return HAUtils::getFloatValue(_currentTemperature, _precision); }

    /**
     * Sets icon of the HVAC.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the HVAC's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    /**
     * Publishes the MQTT message with the given current temperature.
     *
     * @param temperature The temperature to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishCurrentTemperature(const HAUtils::Number temperature);

    /// Features enabled for the HVAC.
    const uint8_t _features;

    /// The precision of temperatures. By default it's `HANumber::PrecisionP1`.
    const NumberPrecision _precision;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The current temperature of the HVAC. By default it's `HAUtils::NumberMax`.
    HAUtils::Number _currentTemperature;
};

#endif
#endif
