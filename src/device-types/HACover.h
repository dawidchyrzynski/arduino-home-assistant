#ifndef AHA_HACOVER_H
#define AHA_HACOVER_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_COVER

#define HACOVER_CALLBACK(name) void (*name)(CoverCommand cmd, HACover* sender)

class HACover : public HABaseDeviceType
{
public:
    static const int16_t DefaultPosition = -32768;

    enum CoverState {
        StateUnknown = 0,
        StateClosed,
        StateClosing,
        StateOpen,
        StateOpening,
        StateStopped
    };

    enum CoverCommand {
        CommandOpen,
        CommandClose,
        CommandStop
    };

    HACover(const char* uniqueId);

    /**
     * Changes state of the cover and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the cover.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(const CoverState state, const bool force = false);

    /**
     * Sets current state of the cover without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the cover.
     */
    inline void setCurrentState(const CoverState state)
        { _currentState = state; }

    /**
     * Returns last known state of the cover.
     * By default state is set to CoverState::StateUnknown
     */
    inline CoverState getCurrentState() const
        { return _currentState; }

    /**
     * Changes position of the cover and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param position New position of the cover.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setPosition(const int16_t position, const bool force = false);

    /**
     * Sets current position of the cover without pushing the value to Home Assistant.
     * This method may be useful if you want to change position before connection
     * with MQTT broker is acquired.
     *
     * @param position New position of the cover.
     */
    inline void setCurrentPosition(const int16_t position)
        { _currentPosition = position; }

    /**
     * Returns last known position of the cover.
     * By default position is set to HACover::DefaultPosition
     */
    inline int16_t getCurrentPosition() const
        { return _currentPosition; }

    /**
     * Sets class of the cover.
     * You can find list of available values here: https://www.home-assistant.io/integrations/cover/
     * 
     * @param class Class name
     */
    inline void setDeviceClass(const char* deviceClass)
        { _class = deviceClass; }

    /**
     * Sets icon of the cover.
     * Any icon from MaterialDesignIcons.com. Prefix name with mdi:, ie mdi:home.
     *
     * @param class Icon name
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets `retain` flag for commands published by Home Assistant.
     * By default it's set to false.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Registers callback that will be called each time the command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same covers.
     *
     * @param callback
     */
    inline void onCommand(HACOVER_CALLBACK(callback))
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
    bool publishState(const CoverState state);
    bool publishPosition(const int16_t position);
    void handleCommand(const char* cmd);

    HACOVER_CALLBACK(_commandCallback);
    CoverState _currentState;
    int16_t _currentPosition;
    const char* _class;
    const char* _icon;
    bool _retain;
};

#endif
#endif
