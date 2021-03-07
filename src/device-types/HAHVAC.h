#ifndef AHA_HVAC_H
#define AHA_HVAC_H

#include "BaseDeviceType.h"

#define HAHVAC_STATE_CALLBACK_BOOL(name) void (*name)(bool)
#define HAHVAC_STATE_CALLBACK_DOUBLE(name) void (*name)(double)

class HAHVAC : public BaseDeviceType
{
public:
    static const char* ActionTopic;
    static const char* AuxCommandTopic;
    static const char* AuxStateTopic;
    static const char* AwayCommandTopic;
    static const char* AwayStateTopic;
    static const char* HoldCommandTopic;
    static const char* HoldStateTopic;
    static const char* TargetTemperatureCommandTopic;
    static const char* TargetTemperatureStateTopic;
    static const char* CurrentTemperatureTopic;

    enum Features {
        DefaultFeatures = 0,
        AuxHeatingFeature = 1,
        AwayModeFeature = 2,
        HoldFeature = 4,
        CurrentTemperatureFeature = 8
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
        DefaultUnit = 1,
        CelsiusUnit,
        FahrenheitUnit
    };

    HAHVAC(const char* uniqueId, Features features, HAMqtt& mqtt);

    virtual void onMqttConnected() override;

    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) override;

    inline void setTemperatureUnit(TemperatureUnit unit)
        { _temperatureUnit = unit; }

    inline void onAuxHeatingStateChanged(HAHVAC_STATE_CALLBACK_BOOL(callback))
        { _auxHeatingCallback = callback; }

    inline bool getAuxHeatingState() const
        { return _auxHeatingState; }

    inline void onAwayStateChanged(HAHVAC_STATE_CALLBACK_BOOL(callback))
        { _awayCallback = callback; }

    inline bool getAwayState() const
        { return _awayState; }

    inline void onHoldStateChanged(HAHVAC_STATE_CALLBACK_BOOL(callback))
        { _holdCallback = callback; }

    inline bool getHoldState() const
        { return _holdState; }

    inline void onTargetTemperatureChanged(HAHVAC_STATE_CALLBACK_DOUBLE(callback))
        { _targetTempCallback = callback; }

    inline double getTargetTemperature() const
        { return _targetTemperature; }

    inline double getCurrentTemperature() const
        { return _currentTemperature; }

    inline void setName(const char* name)
        { _label = name; } // it needs to be called "label" as "name" is already in use

    bool setAction(Action action);
    bool setAuxHeatingState(bool state);
    bool setAwayState(bool state);
    bool setHoldState(bool state);
    bool setCurrentTemperature(double temperature);
    bool setMinTemp(double minTemp);
    bool setMaxTemp(double maxTemp);
    bool setTempStep(double tempStep);
    bool setTargetTemperature(double targetTemperature);

private:
    void publishConfig();
    bool publishAction(Action action);
    bool publishAuxHeatingState(bool state);
    bool publishAwayState(bool state);
    bool publishHoldState(bool state);
    bool publishCurrentTemperature(double temperature);
    bool publishTargetTemperature(double temperature);
    void subscribeTopics();
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    bool writeSerializedData(const char* serializedDevice) const;

    const char* _uniqueId;
    const uint16_t _features;
    TemperatureUnit _temperatureUnit;
    Action _action;
    HAHVAC_STATE_CALLBACK_BOOL(_auxHeatingCallback);
    bool _auxHeatingState;
    HAHVAC_STATE_CALLBACK_BOOL(_awayCallback);
    bool _awayState;
    HAHVAC_STATE_CALLBACK_BOOL(_holdCallback);
    bool _holdState;
    double _currentTemperature;
    double _minTemp;
    double _maxTemp;
    double _tempStep;
    double _targetTemperature;
    HAHVAC_STATE_CALLBACK_DOUBLE(_targetTempCallback);
    const char* _label;
};

#endif
