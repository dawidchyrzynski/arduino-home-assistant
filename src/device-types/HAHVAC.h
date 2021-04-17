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
        ActionFeature = 1,
        AuxHeatingFeature = 2,
        AwayModeFeature = 4,
        HoldFeature = 8
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

    /**
     * Initializes HVAC.
     *
     * @param uniqueId Unique ID of the HVAC. Recommendes characters: [a-z0-9\-_]
     * @param features The list of additional features (flag). For example: `HAHVAC::AuxHeatingFeature | HAHVAC::AwayModeFeature`
     */
    HAHVAC(
        const char* uniqueId,
        uint8_t features = 0
    );
    HAHVAC(
        const char* uniqueId,
        uint8_t features,
        HAMqtt& mqtt
    ); // legacy constructor

    virtual void onMqttConnected() override;

    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) override;

    /**
     * Changes action of the HVAC (it's displayed in the Home Assistant panel).
     *
     * @param action New action.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setAction(Action action);

    /**
     * Returns action that was previously sent to MQTT.
     */
    inline Action getAction() const
        { return _action; }

    /**
     * Changes default temperature unit.
     * Please note that this method must be called before `mqtt.begin(...)`.
     *
     * @param unit See TemperatureUnit enum above
     */
    inline void setTemperatureUnit(TemperatureUnit unit)
        { _temperatureUnit = unit; }

    /**
     * Publishes aux heating state.
     * Please note that HAHVAC::AuxHeatingFeature must be set in the constructor.
     *
     * @param state ON (true) / OFF (false)
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setAuxHeatingState(bool state);

    /**
     * Registers callback that will be called each time the aux heating's state changes.
     * Please note that it's not possible to register multiple callbacks.
     *
     * @param callback
     */
    inline void onAuxHeatingStateChanged(HAHVAC_STATE_CALLBACK_BOOL(callback))
        { _auxHeatingCallback = callback; }

    /**
     * Returns state of the aux heating.
     *
     * @returns ON (true) / OFF (false)
     */
    inline bool getAuxHeatingState() const
        { return _auxHeatingState; }

    /**
     * Publishes away state.
     * Please note that HAHVAC::AwayModeFeature must be set in the constructor.
     *
     * @param state ON (true) / OFF (false)
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setAwayState(bool state);

    /**
     * Registers callback that will be called each time the away's state changes.
     * Please note that it's not possible to register multiple callbacks.
     *
     * @param callback
     */
    inline void onAwayStateChanged(HAHVAC_STATE_CALLBACK_BOOL(callback))
        { _awayCallback = callback; }

    /**
     * Returns away's state.
     *
     * @returns ON (true) / OFF (false)
     */
    inline bool getAwayState() const
        { return _awayState; }

    /**
     * Publishes hold state.
     * Please note that HAHVAC::HoldFeature must be set in the constructor.
     *
     * @param state ON (true) / OFF (false)
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setHoldState(bool state);

    /**
     * Registers callback that will be called each time the hold's state changes.
     * Please note that it's not possible to register multiple callbacks.
     *
     * @param callback
     */
    inline void onHoldStateChanged(HAHVAC_STATE_CALLBACK_BOOL(callback))
        { _holdCallback = callback; }

    /**
     * Returns hold's state.
     *
     * @returns ON (true) / OFF (false)
     */
    inline bool getHoldState() const
        { return _holdState; }

    /**
     * Publishes given target temperature.
     *
     * @param targetTemperature
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setTargetTemperature(double targetTemperature);

    /**
     * Registers callback that will be called each time the target temperature changes
     * Please note that it's not possible to register multiple callbacks.
     *
     * @param callback
     */
    inline void onTargetTemperatureChanged(HAHVAC_STATE_CALLBACK_DOUBLE(callback))
        { _targetTempCallback = callback; }

    /**
     * Returns target temperature.
     *
     * @returns Target temperature or __DBL_MAX__ if temperature is not set.
     */
    inline double getTargetTemperature() const
        { return _targetTemperature; }

    /**
     * Publishes current temperature.
     *
     * @param temperature
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setCurrentTemperature(double temperature);

    /**
     * Return temperature that was previously set by `setCurrentTemperature` method.
     *
     * @returns Temperature or __DBL_MAX__ if temperature is not set.
     */
    inline double getCurrentTemperature() const
        { return _currentTemperature; }

    /**
     * Publishes working mode of the HVAC.
     *
     * @param mode
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setMode(Mode mode);

    /**
     * Same as above but input is the string representation of the mode.
     *
     * @param mode
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setModeFromStr(const char* mode);

    /**
     * Registers callback that will be called each time the mode changes.
     * Please note that it's not possible to register multiple callbacks.
     *
     * @param callback
     */
    inline void onModeChanged(HAHVAC_STATE_CALLBACK_MODE(callback))
        { _modeChangedCallback = callback; }

    /**
     * Returns HVAC's mode.
     *
     * @returns It may be UnknownMode if it's not set.
     */
    inline Mode getMode() const
        { return _currentMode; }

    /**
     * Sets the list of supported modes. By default the list contains all available modes.
     * You can merge multiple modes as following: `setModes(HAHVAC::OffMode | HAHVAC::CoolMode)`
     *
     * @param modes
     */
    inline void setModes(uint8_t modes)
        { _modes = modes; }

    /**
     * Sets `retain` flag for commands published by Home Assistant.
     * By default it's set to false.
     *
     * @param retain
     */
    inline void setRetain(bool retain)
        { _retain = retain; }

    /**
     * Sets the minimum temperature that user will be able to select in Home Assistant panel.
     *
     * @param minTemp
     */
    bool setMinTemp(double minTemp);

    /**
     * Sets the maximum temperature that user will be able to select in Home Assistant panel.
     *
     * @param minTemp
     */
    bool setMaxTemp(double maxTemp);

    /**
     * Sets the step of the temperature's picker in the Home Assistant panel.
     *
     * @param tempStep
     */
    bool setTempStep(double tempStep);

private:
    bool publishAction(Action action);
    bool publishAuxHeatingState(bool state);
    bool publishAwayState(bool state);
    bool publishHoldState(bool state);
    bool publishCurrentTemperature(double temperature);
    bool publishTargetTemperature(double temperature);
    bool publishMode(Mode mode);
    void subscribeTopics();
    uint16_t calculateSerializedLength(const char* serializedDevice) const override;
    uint16_t calculateModesLength() const;
    bool writeSerializedData(const char* serializedDevice) const override;

    const char* _uniqueId;
    const uint8_t _features;
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
    uint8_t _modes;
    HAHVAC_STATE_CALLBACK_MODE(_modeChangedCallback);
    Mode _currentMode;
    bool _retain;
};

#endif
#endif
