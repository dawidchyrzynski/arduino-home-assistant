#ifndef HASWITCH_H
#ifndef NO_HA_SWITCH
#define HASWITCH_H

#include "BaseDeviceType.h"

#define HASWITCH_CALLBACK void (*callback)(bool, HASwitch*)

class HASwitch : public BaseDeviceType
{
public:
    /**
     * Initializes switch.
     *
     * @param name Name of the switch. Recommendes characters: [a-z0-9\-_]
     * @param initialState Initial state of the switch.
                           It will be published right after "config" message in order to update HA state.
     */
    HASwitch(
        const char* name,
        bool initialState,
        HAMqtt& mqtt
    );
    virtual ~HASwitch();

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
        const char* payload,
        const uint16_t& length
    ) override;

    /**
     * Returns name of the switch assigned via constructor.
     */
    inline const char* getName() const
        { return _name; }

    /**
     * Changes state of the switch and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the switch.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(bool state);

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
     * Registers callback that will be called each time the value of the switch changes.
     * Please note that it's not possible to register multiple callbacks for the same switch.
     *
     * @param callback
     */
    void onStateChanged(HASWITCH_CALLBACK);

private:
    void triggerCallback(bool state);
    void publishConfig();
    bool publishState(bool state);
    void subscribeCommandTopic();
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    bool writeSerializedTrigger(const char* serializedDevice) const;

    const char* _name;
    void (*_stateCallback)(bool, HASwitch*);
    bool _currentState;
};

#endif
#endif
