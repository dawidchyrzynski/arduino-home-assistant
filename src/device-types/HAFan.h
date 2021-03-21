#ifndef AHA_FAN_H
#define AHA_FAN_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_FAN

#define HAFAN_STATE_CALLBACK_BOOL(name) void (*name)(bool)
#define HAFAN_STATE_CALLBACK_SPEED(name) void (*name)(Speed)

class HAFan : public BaseDeviceType
{
public:
    static const char* SpeedCommandTopic;
    static const char* SpeedStateTopic;

    enum Features {
        DefaultFeatures = 0,
        SpeedsFeature = 1
    };

    enum Speed {
        UnknownSpeed = 0,
        OffSpeed = 1,
        LowSpeed = 2,
        MediumSpeed = 4,
        HighSpeed = 8
    };

    HAFan(const char* uniqueId, uint8_t features = DefaultFeatures);
    HAFan(const char* uniqueId, uint8_t features, HAMqtt& mqtt); // legacy constructor

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
     * Please note that it's not possible to register multiple callbacks for the same fan.
     *
     * @param callback
     */
    inline void onStateChanged(HAFAN_STATE_CALLBACK_BOOL(callback))
        { _stateCallback = callback; }

    /**
     * Sets the list of supported fan's speeds.
     *
     * @param speeds
     */
    inline void setSpeeds(uint8_t speeds)
        { _speeds = speeds; }

    /**
     * Sets speed of the fan.
     *
     * @param speed
     */
    bool setSpeed(Speed speed);

    /**
     * Sets speed of the fan based on the name of the mode.
     *
     * @param speed
     */
    bool setSpeedFromStr(const char* speed);

    /**
     * Returns current speed of the fan.
     */
    inline Speed getSpeed() const
        { return _currentSpeed; }

    /**
     * Registers callback that will be called each time the speed of the fan changes.
     * Please note that it's not possible to register multiple callbacks for the same fan.
     *
     * @param callback
     */
    inline void onSpeedChanged(HAFAN_STATE_CALLBACK_SPEED(callback))
        { _speedCallback = callback; }

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
    bool publishState(bool state);
    bool publishSpeed(Speed speed);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    uint16_t calculateSpeedsLength() const;
    bool writeSerializedData(const char* serializedDevice) const override;

    const uint8_t _features;
    uint8_t _speeds;
    bool _currentState;
    HAFAN_STATE_CALLBACK_BOOL(_stateCallback);
    Speed _currentSpeed;
    HAFAN_STATE_CALLBACK_SPEED(_speedCallback);
    const char* _label;
    bool _retain;
};

#endif
#endif
