#ifndef AHA_LIGHT_H
#define AHA_LIGHT_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_LIGHT

#define HALIGHT(name) void (*name)(bool)
#define HALIGHT_STATE_CALLBACK_BOOL(name) void (*name)(bool)

class HALight : public BaseDeviceType
{
public:
    static const char* PercentageCommandTopic;
    static const char* PercentageStateTopic;

    enum Features {
        DefaultFeatures = 0,
    };

    HALight(const char* uniqueId, uint8_t features = DefaultFeatures);

    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) override;

    /**
     * Changes state of the light and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the light (on - true, off - false).
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(bool state, bool force = false);

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
     * Returns last known state of the light.
     * If setState method wasn't called the initial value will be returned.
     */
    inline bool getState() const
        { return _currentState; }

    /**
     * Registers callback that will be called each time the state of the light changes.
     * Please note that it's not possible to register multiple callbacks for the same light.
     *
     * @param callback
     */
    inline void onStateChanged(HALIGHT_STATE_CALLBACK_BOOL(callback))
        { _stateCallback = callback; }

    /**
     * Sets `retain` flag for commands published by Home Assistant.
     * By default it's set to false.
     *
     * @param retain
     */
    inline void setRetain(bool retain)
        { _retain = retain; }

protected:
    bool publishState(bool state);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;

    const uint8_t _features;
    bool _currentState;
    HALIGHT_STATE_CALLBACK_BOOL(_stateCallback);
    bool _retain;
};

#endif
#endif
