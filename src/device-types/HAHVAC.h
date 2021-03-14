#ifndef AHA_HVAC_H
#define AHA_HVAC_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_HVAC

#define HAHVAC_STATE_CALLBACK_BOOL(name) void (*name)(bool)
#define HAHVAC_STATE_CALLBACK_DOUBLE(name) void (*name)(double)
#define HAHVAC_STATE_CALLBACK_MODE(name) void (*name)(HAHVAC::Mode)

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
    static const char* ModeCommandTopic;
    static const char* ModeStateTopic;

    enum Features {
        DefaultFeatures = 0,
        AuxHeatingFeature = 1,
        AwayModeFeature = 2,
        HoldFeature = 4,
        CurrentTemperatureFeature = 8
    };

    enum Action {
        UnknownAction = 0,
        OffAction,
        HeatingAction,
        CoolingAction,
        DryingAction,
        IdleAction,
        FanAction
    };

    enum Mode {
        UnknownMode = 0,
        OffMode = 1,
        AutoMode = 2,
        CoolMode = 4,
        HeatMode = 8,
        DryMode = 16,
        FanOnlyMode = 32
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
        { _label = name; } // it needs to be called "label" as "_name" is already in use

    inline void setModes(uint8_t modes)
        { _modes = modes; }

    inline Mode getMode() const
        { return _currentMode; }

    inline void onModeChanged(HAHVAC_STATE_CALLBACK_MODE(callback))
        { _modeChangedCallback = callback; }

    bool setAction(Action action);
    bool setAuxHeatingState(bool state);
    bool setAwayState(bool state);
    bool setHoldState(bool state);
    bool setCurrentTemperature(double temperature);
    bool setMinTemp(double minTemp);
    bool setMaxTemp(double maxTemp);
    bool setTempStep(double tempStep);
    bool setTargetTemperature(double targetTemperature);
    bool setMode(Mode mode);
    bool setModeFromStr(const char* mode);

private:
    void publishConfig();
    bool publishAction(Action action);
    bool publishAuxHeatingState(bool state);
    bool publishAwayState(bool state);
    bool publishHoldState(bool state);
    bool publishCurrentTemperature(double temperature);
    bool publishTargetTemperature(double temperature);
    bool publishMode(Mode mode);
    void subscribeTopics();
    uint16_t calculateSerializedLength(const char* serializedDevice) const;
    uint16_t calculateModesLength() const;
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
    HAHVAC_STATE_CALLBACK_DOUBLE(_targetTempCallback);
    double _targetTemperature;
    const char* _label;
    uint8_t _modes;
    HAHVAC_STATE_CALLBACK_MODE(_modeChangedCallback);
    Mode _currentMode;

};

#endif
#endif
