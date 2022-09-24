#ifndef AHA_HALIGHT_H
#define AHA_HALIGHT_H

#include "HABaseDeviceType.h"
#include "../utils/HAUtils.h"

#ifndef EX_ARDUINOHA_LIGHT

#define HALIGHT_STATE_CALLBACK(name) void (*name)(bool state, HALight* sender)
#define HALIGHT_BRIGHTNESS_CALLBACK(name) void (*name)(uint8_t brightness, HALight* sender)

/**
 * 
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/light.mqtt/
 */
class HALight : public HABaseDeviceType
{
public:
    static const uint8_t DefaultBrightnessScale;

    enum Features {
        DefaultFeatures = 0,
        BrightnessFeature = 1
    };

    /**
     * @param uniqueId The unique ID of the light. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the light.
     */
    HALight(const char* uniqueId, const Features features = DefaultFeatures);

    /**
     * Changes state of the light and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the light.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setState(const bool state, const bool force = false);

    /**
     * Changes the brightness of the light and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param brightness The new brightness of the light.
     * @param force Forces to update the state without comparing it to a previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setBrightness(const uint8_t brightness, const bool force = false);

    /**
     * Alias for `setState(true)`.
     */
    inline bool turnOn()
        { return setState(true); }

    /**
     * Alias for `setState(false)`.
     */
    inline bool turnOff()
        { return setState(false); }

    /**
     * Sets current state of the light without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the light.
     */
    inline void setCurrentState(const bool state)
        { _currentState = state; }

    /**
     * Returns last known state of the light.
     * By default it's `false`.
     */
    inline bool getCurrentState() const
        { return _currentState; }  

    /**
     * Sets the current brightness of the light without pushing the value to Home Assistant.
     * This method may be useful if you want to change the brightness before the connection
     * with the MQTT broker is acquired.
     *
     * @param brightness The new brightness of the light.
     */
    inline void setCurrentBrightness(const uint8_t brightness)
        { _currentBrightness = brightness; }

    /**
     * Returns the last known brightness of the light.
     * By default brightness is set to `0`.
     */
    inline uint8_t getCurrentBrightness() const
        { return _currentBrightness; }  

    /**
     * Sets icon of the light.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the light's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the light state.
     * In this mode the light state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Sets the maximum brightness value that can be set via HA panel.
     * By default it's `255`.
     * 
     * @param scale The maximum value of the brightness.
     */
    inline void setBrightnessScale(const uint8_t scale)
        { _brightnessScale = scale; }

    /**
     * Registers callback that will be called each time the state command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     *
     * @param callback
     */
    inline void onStateCommand(HALIGHT_STATE_CALLBACK(callback))
        { _stateCallback = callback; }

    /**
     * Registers callback that will be called each time the brightness command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     *
     * @param callback
     */
    inline void onBrightnessCommand(HALIGHT_BRIGHTNESS_CALLBACK(callback))
        { _brightnessCallback = callback; }

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
     * Publishes the MQTT message with the given state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishState(const bool state);

    /**
     * Publishes the MQTT message with the given brightness.
     *
     * @param brightness The brightness to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishBrightness(const uint8_t brightness);

    /**
     * Parses the given state command and executes the callback with proper value.
     * 
     * @param cmd The string representation of the command.
     * @param length Length of the command.
     */
    void handleStateCommand(const char* cmd, const uint16_t length);

    /**
     * Parses the given brightness command and executes the callback with proper value.
     * 
     * @param cmd The string representation of the command.
     * @param length Length of the command.
     */
    void handleBrightnessCommand(const char* cmd, const uint16_t length);

    /// Features enabled for the light.
    const uint8_t _features;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the light (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// The maximum value of the brightness. By default it's 255.
    uint8_t _brightnessScale;

    /// The current state of the light. By default it's `false`.
    bool _currentState;

    /// The current brightness of the light. By default it's `0`.
    uint8_t _currentBrightness;

    /// The callback that will be called when the state command is received from the HA.
    HALIGHT_STATE_CALLBACK(_stateCallback);

    /// The callback that will be called when the brightness command is received from the HA.
    HALIGHT_BRIGHTNESS_CALLBACK(_brightnessCallback);  
};

#endif
#endif
