#ifndef AHA_HASWITCH_H
#define AHA_HASWITCH_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_SWITCH

#define HASWITCH_CALLBACK(name) void (*name)(bool, HASwitch*)

class HASwitch : public BaseDeviceType
{
public:
    /**
     * Initializes switch.
     *
     * @param uniqueId Unique ID of the switch. Recommendes characters: [a-z0-9\-_]
     * @param initialState Initial state of the switch.
                           It will be published right after "config" message in order to update HA state.
     */
    HASwitch(
        const char* uniqueId,
        bool initialState
    );
    HASwitch(
        const char* uniqueId,
        bool initialState,
        HAMqtt& mqtt
    ); // legacy constructor

    /**
     * Publishes configuration of the sensor to the MQTT.
     */
    virtual void onMqttConnected() override;

    /**
     * Processes message received from the MQTT broker.
     * The method updates state of the switch (if message matches switch'es topic).
     */
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) override;

    /**
     * Changes state of the switch and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the switch.
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
     * Returns last known state of the switch.
     * If setState method wasn't called the initial value will be returned.
     */
    inline bool getState() const
        { return _currentState; }

    /**
     * Registers callback that will be called each time the state of the switch changes.
     * Please note that it's not possible to register multiple callbacks for the same switch.
     *
     * @param callback
     */
    inline void onStateChanged(HASWITCH_CALLBACK(callback))
        { _stateCallback = callback; }

    /**
     * Registers callback that will be called before state of the switch changes.
     * The state passed to callback is a new state that is going to be set.
     *
     * @param callback
     */
    inline void onBeforeStateChanged(HASWITCH_CALLBACK(callback))
        { _beforeStateCallback = callback; }

    /**
     * Sets icon of the switch, e.g. `mdi:home`.
     *
     * @param icon Material Design Icon name with mdi: prefix.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets `retain` flag for commands published by Home Assistant.
     * By default it's set to false.
     *
     * @param retain
     */
    inline void setRetain(bool retain)
        { _retain = retain; }

private:
    bool publishState(bool state);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;

    HASWITCH_CALLBACK(_stateCallback);
    HASWITCH_CALLBACK(_beforeStateCallback);
    bool _currentState;
    const char* _icon;
    bool _retain;
};

#endif
#endif
