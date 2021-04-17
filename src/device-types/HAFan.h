#ifndef AHA_FAN_H
#define AHA_FAN_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_FAN

#define HAFAN_STATE_CALLBACK_BOOL(name) void (*name)(bool)
#define HAFAN_STATE_CALLBACK_SPEED(name) void (*name)(uint16_t)
#define HAFAN_STATE_CALLBACK_SPEED_DEPRECATED(name) void (*name)(Speed)

class HAFan : public BaseDeviceType
{
public:
    static const char* PercentageCommandTopic;
    static const char* PercentageStateTopic;

    enum Features {
        DefaultFeatures = 0,
        SpeedsFeature = 1
    };

    // @deprecated
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
    AHA_DEPRECATED(inline void setSpeeds(uint8_t speeds))
        { (void)speeds; }

    /**
     * Sets speed of the fan.
     *
     * @param speed
     */
    bool setSpeed(uint16_t speed);

    /**
     * Returns current speed of the fan.
     */
    inline uint16_t getSpeed() const
        { return _currentSpeed; }

    /**
     * Registers callback that will be called each time the speed of the fan changes.
     * Please note that it's not possible to register multiple callbacks for the same fan.
     *
     * @param callback
     */
    inline void onSpeedChanged(HAFAN_STATE_CALLBACK_SPEED(callback))
        { _speedCallback = callback; }

    AHA_DEPRECATED(inline void onSpeedChanged(HAFAN_STATE_CALLBACK_SPEED_DEPRECATED(callback)))
        { (void)callback; }

    /**
     * Sets `retain` flag for commands published by Home Assistant.
     * By default it's set to false.
     *
     * @param retain
     */
    inline void setRetain(bool retain)
        { _retain = retain; }

    /**
     * Sets minimum range for slider in the HA panel.
     *
     * @param min
     */
    inline void setSpeedRangeMin(uint16_t min)
        { _speedRangeMin = min; }

    /**
     * Sets maximum range for slider in the HA panel.
     *
     * @param min
     */
    inline void setSpeedRangeMax(uint16_t max)
        { _speedRangeMax = max; }

protected:
    bool publishState(bool state);
    bool publishSpeed(uint16_t speed);
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    bool writeSerializedData(const char* serializedDevice) const override;

    const uint8_t _features;
    bool _currentState;
    HAFAN_STATE_CALLBACK_BOOL(_stateCallback);
    uint16_t _currentSpeed;
    HAFAN_STATE_CALLBACK_SPEED(_speedCallback);
    bool _retain;
    uint16_t _speedRangeMin;
    uint16_t _speedRangeMax;
};

#endif
#endif
