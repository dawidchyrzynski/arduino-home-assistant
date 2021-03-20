#ifndef AHA_FAN_H
#define AHA_FAN_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_FAN

#define HAFAN_STATE_CALLBACK_BOOL(name) void (*name)(bool)

class HAFan : public BaseDeviceType
{
public:
    enum Speed {
        OffSpeed = 1,
        LowSpeed,
        MediumSpeed,
        HighSpeed
    };

    HAFan(const char* uniqueId);
    HAFan(const char* uniqueId, HAMqtt& mqtt); // legacy constructor

    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) override;

    /**
     * Changes state of the fan and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the fan (on - true, off - false).
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
     * Returns last known state of the fan.
     * If setState method wasn't called the initial value will be returned.
     */
    inline bool getState() const
        { return _currentState; }

    /**
     * Registers callback that will be called each time the state of the fan changes.
     * Please note that it's not possible to register multiple callbacks for the same switch.
     *
     * @param callback
     */
    inline void onStateChanged(HAFAN_STATE_CALLBACK_BOOL(callback))
        { _stateCallback = callback; }

protected:
    bool publishState(bool state);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;

    bool _currentState;
    HAFAN_STATE_CALLBACK_BOOL(_stateCallback);


    const char* _label;
    bool _retain;
};

#endif
#endif
