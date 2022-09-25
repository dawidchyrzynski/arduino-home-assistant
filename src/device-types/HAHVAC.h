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
     */
    HAHVAC(const char* uniqueId, const uint8_t features = DefaultFeatures);

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

    /// Features enabled for the HVAC.
    const uint8_t _features;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain; 
};

#endif
#endif
