#ifndef AHA_COVER_H
#define AHA_COVER_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_COVER

class HACover : public BaseDeviceType
{
public:
    enum CoverState {
        StateUnknown = 0,
        StateClosed,
        StateClosing,
        StateOpen,
        StateOpenning,
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
     * Changes state of the covers and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the fan (on - true, off - false).
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(CoverState state, bool force = false);

    /**
     * Sets name that wil be displayed in the Home Assistant panel.
     *
     * @param name
     */
    inline void setName(const char* name)
        { _label = name; } // it needs to be called "label" as "_name" is already in use

    /**
     * Sets `retain` flag for commands published by Home Assistant.
     * By default it's set to false.
     *
     * @param retain
     */
    inline void setRetain(bool retain)
        { _retain = retain; }

protected:
    bool publishState(CoverState state);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;

    CoverState _currentState;
    const char* _label;
    bool _retain;
};

#endif
#endif
