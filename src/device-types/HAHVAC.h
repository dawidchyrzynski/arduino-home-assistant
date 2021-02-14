#ifndef AHA_HVAC_H
#define AHA_HVAC_H

#include "BaseDeviceType.h"

#define HAHVAC_STATE_CALLBACK(name) void (*name)(bool)

class HAHVAC : public BaseDeviceType
{
public:
    static const char* ActionTopic;
    static const char* AuxCommandTopic;
    static const char* AuxStateTopic;
    static const char* AwayCommandTopic;
    static const char* AwayStateTopic;

    enum Features {
        AuxHeatingFeature = 1,
        AwayModeFeature = 2,
        FanFeature = 4
    };

    enum Action {
        OffAction = 1,
        HeatingAction,
        CoolingAction,
        DryingAction,
        IdleAction,
        FanAction
    };

    enum TemperatureUnit {
        SystemUnit = 1,
        CelsiusUnit,
        FahrenheitUnit
    };

    HAHVAC(const char* uniqueId, HAMqtt& mqtt);

    /**
     * Publishes configuration of the HVAC to the MQTT.
     */
    virtual void onMqttConnected() override;

    /**
     * Processes message received from the MQTT broker.
     */
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) override;

    inline void setFeatures(uint16_t features)
        { _features = features; }

    inline void setTemperatureUnit(TemperatureUnit unit)
        { _temperatureUnit = unit; }

    inline void onAuxHeatingStateChanged(HAHVAC_STATE_CALLBACK(callback))
        { _auxHeatingCallback = callback; }

    inline bool getAuxHeatingState() const
        { return _auxHeatingState; }

    inline void onAwayStateChanged(HAHVAC_STATE_CALLBACK(callback))
        { _awayCallback = callback; }

    inline bool getAwayState() const
        { return _awayState; }

    bool setAction(Action action);
    bool setAuxHeatingState(bool state);
    bool setAwayState(bool state);

private:
    void publishConfig();
    bool publishAction(Action action);
    bool publishAuxHeatingState(bool state);
    bool publishAwayState(bool state);
    bool publishMessage(const char* topic, const char* data);
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    bool writeSerializedData(const char* serializedDevice) const;

    const char* _uniqueId;
    uint16_t _features;
    TemperatureUnit _temperatureUnit;
    Action _action;
    HAHVAC_STATE_CALLBACK(_auxHeatingCallback);
    bool _auxHeatingState;
    HAHVAC_STATE_CALLBACK(_awayCallback);
    bool _awayState;
};

#endif
