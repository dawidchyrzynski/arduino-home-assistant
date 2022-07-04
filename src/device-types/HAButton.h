#ifndef AHA_HABUTTON_H
#define AHA_HABUTTON_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_BUTTON

#define HABUTTON_CALLBACK(name) void (*name)(HAButton* sender)

class HAButton : public HABaseDeviceType
{
public:
    HAButton(const char* uniqueId);

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/button/#device-class
     * 
     * @param class Class name
     */
    inline void setDeviceClass(const char* deviceClass)
        { _class = deviceClass; }

    /**
     * Sets icon of the sensor.
     * Any icon from MaterialDesignIcons.com. Prefix name with mdi:, ie mdi:home.
     *
     * @param class Icon name
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the button's command.
     * If set to true the command produced by Home Assistant will be retained.
     * 
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Registers callback that will be called each time the press command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same button.
     *
     * @param callback
     */
    inline void onPress(HABUTTON_CALLBACK(callback))
        { _commandCallback = callback; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    bool publishState(bool state);

    const char* _class;
    const char* _icon;
    bool _retain;
    HABUTTON_CALLBACK(_commandCallback);
};

#endif
#endif
