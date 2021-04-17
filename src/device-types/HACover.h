#ifndef AHA_COVER_H
#define AHA_COVER_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_COVER

#define HACOVER_CALLBACK(name) void (*name)(CoverCommand cmd)

class HACover : public BaseDeviceType
{
public:
    static const char* PositionTopic;

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
    HACover(const char* uniqueId, HAMqtt& mqtt); // legacy constructor

    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) override;

    /**
     * Changes state of the cover and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the cover.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(CoverState state, bool force = false);

    /**
     * Sets current state of the cover without pushing the state to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the cover.
     */
    inline void setCurrentState(CoverState state)
        { _currentState = state; }

    /**
     * Changes position of the cover and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param position New position of the cover.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setPosition(int16_t position);

    /**
     * Sets current position of the cover without pushing the value to Home Assistant.
     * This method may be useful if you want to change position before connection
     * with MQTT broker is acquired.
     *
     * @param position New position of the cover.
     */
    inline void setCurrentPosition(int16_t position)
        { _currentPosition = position; }

    /**
     * Sets `retain` flag for commands published by Home Assistant.
     * By default it's set to false.
     *
     * @param retain
     */
    inline void setRetain(bool retain)
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
    bool publishState(CoverState state);
    bool publishPosition(int16_t position);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;
    void handleCommand(const char* cmd);

    HACOVER_CALLBACK(_commandCallback);
    CoverState _currentState;
    int16_t _currentPosition;
    bool _retain;
};

#endif
#endif
