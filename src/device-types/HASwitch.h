#ifndef AHA_HASWITCH_H
#define AHA_HASWITCH_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_SWITCH

#define HASWITCH_CALLBACK(name) void (*name)(bool state, HASwitch* sender)

class HASwitch : public HABaseDeviceType
{
public:
    HASwitch(const char* uniqueId);

    /**
     * Sets class of the switch.
     * You can find list of available values here: https://www.home-assistant.io/integrations/switch/#device-class
     * 
     * @param class Class name
     */
    inline void setDeviceClass(const char* deviceClass)
        { _class = deviceClass; }

    /**
     * Sets icon of the switch.
     * Any icon from MaterialDesignIcons.com. Prefix name with mdi:, ie mdi:home.
     *
     * @param class Icon name
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the switch command.
     * If set to true the command produced by Home Assistant will be retained.
     * 
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the switch state.
     *
     * @param optimistic
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Changes state of the switch and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the switch.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(const bool state, const bool force = false);

    /**
     * Alias for setState(true).
     */
    inline bool turnOn()
        { return setState(true); }

    /**
     * Alias for setState(false).
     */
    inline bool turnOff()
        { return setState(false); }

    /**
     * Sets current state of the switch without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the switch.
     */
    inline void setCurrentState(const bool state)
        { _currentState = state; }

    /**
     * Returns last known state of the switch.
     * If setState method wasn't called the initial value will be returned.
     */
    inline bool getCurrentState() const
        { return _currentState; }

    /**
     * Registers callback that will be called each time the on/off command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same switch.
     *
     * @param callback
     */
    inline void onCommand(HASWITCH_CALLBACK(callback))
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
    bool publishState(const bool state);
    void handleCommand(const char* cmd);

    const char* _class;
    const char* _icon;
    bool _retain;
    bool _optimistic;
    bool _currentState;
    HASWITCH_CALLBACK(_commandCallback);
};

#endif
#endif
