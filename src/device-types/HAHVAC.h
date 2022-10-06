#ifndef AHA_HAHVAC_H
#define AHA_HAHVAC_H

#include "HABaseDeviceType.h"
#include "../utils/HAUtils.h"

#ifndef EX_ARDUINOHA_HVAC

#define HAHVAC_CALLBACK_BOOL(name) void (*name)(bool, HAHVAC*)

/**
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/climate.mqtt/
 */
class HAHVAC : public HABaseDeviceType
{
public:
    enum Features {
        DefaultFeatures = 0,
        ActionFeature = 1,
        AuxHeatingFeature = 2
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

    enum TemperatureUnit {
        DefaultUnit = 1,
        CelsiusUnit,
        FahrenheitUnit
    };

    /**
     * @param uniqueId The unique ID of the HVAC. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the HVAC.
     * @param precision The precision of temperatures reported by the HVAC.
     */
    HAHVAC(
        const char* uniqueId,
        const uint16_t features = DefaultFeatures,
        const NumberPrecision precision = PrecisionP1
    );

    /**
     * Changes current temperature of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param temperature New current temperature.
     * @param force Forces to update the temperature without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setCurrentTemperature(const float temperature, const bool force = false);

    /**
     * Sets current temperature of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to change temperature before connection
     * with MQTT broker is acquired.
     *
     * @param temperature New current temperature.
     */
    inline void setCurrentCurrentTemperature(const float temperature)
        { _currentTemperature = HAUtils::processFloatValue(temperature, _precision); }

    /**
     * Returns last known current temperature of the HVAC.
     * If setCurrentTemperature method wasn't called the initial value will be returned.
     */
    inline float getCurrentTemperature() const
        { return HAUtils::getFloatValue(_currentTemperature, _precision); }

    /**
     * Changes action of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param action New action.
     * @param force Forces to update the action without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setAction(const Action action, const bool force = false);

    /**
     * Sets action of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to the action before connection
     * with MQTT broker is acquired.
     *
     * @param action New action.
     */
    inline void setCurrentAction(const Action action)
        { _action = action; }

    /**
     * Returns last known action of the HVAC.
     * If setAction method wasn't called the initial value will be returned.
     */
    inline Action getAction() const
        { return _action; }

    /**
     * Changes state of the aux heating and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state The new state.
     * @param force Forces to update the state without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setAuxState(const bool state, const bool force = false);

    /**
     * Sets aux heating state without publishing it to Home Assistant.
     * This method may be useful if you want to change the state before connection
     * with MQTT broker is acquired.
     *
     * @param state The new state.
     */
    inline void setCurrentAuxState(const bool state)
        { _auxState = state; }

    /**
     * Returns last known state of the aux heating.
     * If setAuxState method wasn't called the initial value will be returned.
     */
    inline bool getAuxState() const
        { return _auxState; }

    /**
     * Sets icon of the HVAC.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the HVAC's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Changes the temperature unit.
     *
     * @param unit See the TemperatureUnit enum above.
     */
    inline void setTemperatureUnit(TemperatureUnit unit)
        { _temperatureUnit = unit; }

    /**
     * Sets the minimum temperature that can be set from the Home Assistant panel.
     *
     * @param min The minimum value.
     */
    inline void setMinTemp(const float min)
        { _minTemp = HAUtils::processFloatValue(min, _precision); }

    /**
     * Sets the maximum temperature that can be set from the Home Assistant panel.
     *
     * @param min The maximum value.
     */
    inline void setMaxTemp(const float max)
        { _maxTemp = HAUtils::processFloatValue(max, _precision); }

    /**
     * Sets the step of the temperature that can be set from the Home Assistant panel.
     *
     * @param step The setp value. By default it's `1`.
     */
    inline void setTempStep(const float step)
        { _tempStep = HAUtils::processFloatValue(step, _precision); }

    /**
     * Registers callback that will be called each time the aux state command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same HVAC.
     *
     * @param callback
     */
    inline void onAuxStateCommand(HAHVAC_CALLBACK_BOOL(callback))
        { _auxCallback = callback; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    /**
     * Publishes the MQTT message with the given current temperature.
     *
     * @param temperature The temperature to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishCurrentTemperature(const HAUtils::Number temperature);

    /**
     * Publishes the MQTT message with the given action.
     *
     * @param action The action to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishAction(const Action action);

    /**
     * Publishes the MQTT message with the given aux heating state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishAuxState(const bool state);

    /**
     * Parses the given aux state command and executes the callback with proper value.
     * 
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleAuxStateCommand(const uint8_t* cmd, const uint16_t length);

    /// Features enabled for the HVAC.
    const uint16_t _features;

    /// The precision of temperatures. By default it's `HANumber::PrecisionP1`.
    const NumberPrecision _precision;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The current temperature of the HVAC. By default it's `HAUtils::NumberMax`.
    HAUtils::Number _currentTemperature;

    /// The current action of the HVAC. By default it's `HAHVAC::UnknownAction`.
    Action _action;

    /// The temperature unit for the HVAC. By default it's `HAHVAC::DefaultUnit`.
    TemperatureUnit _temperatureUnit;

    /// The minimum temperature that can be set.
    HAUtils::Number _minTemp;

    /// The maximum temperature that can be set.
    HAUtils::Number _maxTemp;

    /// The step of the temperature that can be set.
    HAUtils::Number _tempStep;

    /// Callback that will be called when the aux command is received from the HA.
    HAHVAC_CALLBACK_BOOL(_auxCallback);

    /// The state of the aux heating. By default it's `false`.
    bool _auxState;
};

#endif
#endif
