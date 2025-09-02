#ifndef AHA_HAALARMPANEL_H
#define AHA_HAALARMPANEL_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_ALARMPANEL

#define HAALARMPANEL_CALLBACK(name) void (*name)(const uint8_t* cmd, const uint16_t length, HAAlarmControlPanel* sender)

class HASerializerArray;

/**
 * Alarm Control Panel lets you control your security system.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/
 */
class HAAlarmControlPanel : public HABaseDeviceType
{
public:

    /// The list of features available in the panel. They're used in the constructor.
    enum SupportedFeatures {
        DefaultFeatures = 0,
        ArmHome = 1,
        ArmAway = 2,
        ArmNight = 4,
        ArmVacation = 8,
        ArmCustomBypass = 16,
        Trigger = 32
    };

    /// The list of possible states that the panel is set to.
    enum PanelState {
        UnknownState = 0,
        Disarmed,
        ArmedHome,
        ArmedAway,
        ArmedNight,
        ArmedVacation,
        ArmedCustomBypass,
        Pending,
        Triggered,
        Arming,
        Disarming
    };

    /**
     * @param uniqueId The unique ID of the HVAC. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the panel.
     */
    HAAlarmControlPanel(
        const char* uniqueId,
        const uint16_t features = DefaultFeatures
    );

    /**
     * Frees memory allocated for the arrays serialization.
     */
    ~HAAlarmControlPanel();

    /**
     * Sets the state of the alarm panel
     *
     * @param state The new state.
     */
    bool setPanelState(const PanelState state, const bool force = false);

    /**
     * Retrieves the last known state of the alarm panel
     */
    inline bool getPanelState() const
        { return _panelState; }

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
     * Sets enable by default flag for the entity.
     * If set to `false` the entity will be disabled when added to Home Assisant. The user will have
     * to enable it to use the entity. Useful for control entities that are convenient to have for advanced
     * users, but most people might not need.
     *
     * @param enableByDefault
     */
    inline void setEnableByDefault(const bool enableByDefault)
        { _enableByDefault = enableByDefault; }

    /**
     * Sets the entity category for the panel.
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#entity_category
     *
     * @param entityCategory The category name.
     */
    inline void setEntityCategory(const char* entityCategory)
        { _entityCategory = entityCategory; }

    /**
     * If defined, specifies a code to enable or disable the alarm in the frontend. Note that the code is validated
     * locally and blocks sending MQTT messages to the remote device. 
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#code
     *
     * @param payload The payload to be sent to activate the mode.
     */
    inline void setCode(const char* code)
        { _code = code; }

    /**
     * Sets whether the code is required to arm the panel.
     * If true, the code is required to arm the alarm. If false, the code will not be validated. Default is true.
     *
     * @param required
     */
    inline void setCodeArmRequired(const bool required)
        { _codeArmRequired = required; }

    /**
     * Sets whether the code is required to disarm the panel.
     * If true, the code is required to disarm the alarm. If false, the code will not be validated. Default is true.
     *
     * @param required
     */
    inline void setCodeDisarmRequired(const bool required)
        { _codeDisarmRequired = required; }

    /**
     * Sets whether the code is required to trigger the alarm.
     * If true, the code is required to trigger the alarm. If false, the code will not be validated. Default is true.
     *
     * @param required
     */
    inline void setCodeTriggerRequired(const bool required)
        { _codeTriggerRequired = required; }


    /**
     * Sets the payload to send to the panel to arm away.
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#payload_arm_away
     *
     * @param payload The payload to be sent to activate the mode.
     */
    inline void setPayloadArmAway(const char* payload)
        { _payloadArmAway = payload; }

    /**
     * Sets the payload to send to the panel to arm home.
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#payload_arm_home
     *
     * @param payload The payload to be sent to activate the mode.
     */
    inline void setPayloadArmHome(const char* payload)
        { _payloadArmHome = payload; }

    /**
     * Sets the payload to send to the panel to arm night.
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#payload_arm_night
     *
     * @param payload The payload to be sent to activate the mode.
     */
    inline void setPayloadArmNight(const char* payload)
        { _payloadArmNight = payload; }
    
    /**
     * Sets the payload to send to the panel to arm vacation.
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#payload_arm_vacation
     *
     * @param payload The payload to be sent to activate the mode.
     */
    inline void setPayloadArmVacation(const char* payload)
        { _payloadArmVacation = payload; }
    
    /**
     * Sets the payload to send to the panel to arm custom bypass.
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#payload_arm_custom_bypass
     *
     * @param payload The payload to be sent to activate the mode.
     */
    inline void setPayloadCustomBypass(const char* payload)
        { _payloadArmCustomBypass = payload; }

    /**
     * Sets the payload to send to the panel to disarmit.
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#payload_disarm
     *
     * @param payload The payload to be sent to activate the mode.
     */
    inline void setPayloadDisarm(const char* payload)
        { _payloadDisarm = payload; }

    /**
     * Sets the payload to trigger the alarm.
     * See: https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/#payload_trigger
     *
     * @param payload The payload to be sent to activate the mode.
     */
    inline void setPayloadTrigger(const char* payload)
        { _payloadTrigger = payload; }

    /**
     * Registers callback that will be called each time the user issues a command (e.g. to arm, disarm) to the panel via HA.
     *
     * @param callback
     */
    inline void onCommand(HAALARMPANEL_CALLBACK(callback))
        { _commandCallback = callback; }


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
     * @param state The alarm state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishPanelState(const PanelState state);

    /**
     * Parses the given command message and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleCommand(const uint8_t* cmd, const uint16_t length);

    /// The supported features of the alarm paenl.
    const uint16_t _features;

    /// The icon of the entity. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The panel state.
    PanelState _panelState;

    /// If defined, specifies a code to enable or disable the alarm in the frontend. 
    const char* _code;

    /// If true the code is required to arm the alarm. If false the code is not validated. Default is true.
    bool _codeArmRequired;

    /// If true the code is required to diarmarm the alarm. If false the code is not validated. Default is true.
    bool _codeDisarmRequired;

    /// If true the code is required to trigger the alarm. If false the code is not validated. Default is true.
    bool _codeTriggerRequired;

    /// If true the panel is enabled when it's first added to HA. If false it is disabled when first added. Default is true.
    bool _enableByDefault;

    /// The entity category for the entity. It can be nullptr.
    const char* _entityCategory;

    /// Callback that will be called when the HA sends a command to arm, disarm, etc. Will contain the corresponding payload.
    HAALARMPANEL_CALLBACK(_commandCallback);

    /// The payload to set armed-away mode on your Alarm Panel.
    const char* _payloadArmAway;

    /// The payload to set armed-home mode on your Alarm Panel.
    const char* _payloadArmHome;

    /// The payload to set armed-night mode on your Alarm Panel.
    const char* _payloadArmNight;

    /// The payload to set armed-vacation mode on your Alarm Panel.
    const char* _payloadArmVacation;

    /// The payload to set armed-custom-bypass mode on your Alarm Panel.
    const char* _payloadArmCustomBypass;

    /// The payload to disarm your Alarm Panel.
    const char* _payloadDisarm;

    /// The payload to trigger the alarm on your Alarm Panel.
    const char* _payloadTrigger;

    /// The serializer for the supported features.
    HASerializerArray* _supportedFeaturesSerializer;
};

#endif
#endif
