#ifndef AHA_HADEVICETRIGGER_H
#define AHA_HADEVICETRIGGER_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_DEVICE_TRIGGER

class HADeviceTrigger : public HABaseDeviceType
{
public:
    enum TriggerType {
        ButtonShortPressType = 1,
        ButtonShortReleaseType,
        ButtonLongPressType,
        ButtonLongReleaseType,
        ButtonDoublePressType,
        ButtonTriplePressType,
        ButtonQuadruplePressType,
        ButtonQuintuplePressType
    };

    enum TriggerSubtype {
        TurnOnSubtype = 1,
        TurnOffSubtype,
        Button1Subtype,
        Button2Subtype,
        Button3Subtype,
        Button4Subtype,
        Button5Subtype,
        Button6Subtype
    };

    /**
     * Creates a new trigge based on the given type and subtype.
     * Please note that combination of type and subtype needs to be unique in a scope of the device.
     *
     * The list of built-in types is available here: https://www.home-assistant.io/integrations/device_trigger.mqtt/#type
     * The list of built-in subtypes is available here: https://www.home-assistant.io/integrations/device_trigger.mqtt/#subtype
     */
    HADeviceTrigger(const char* type, const char* subtype);
    HADeviceTrigger(TriggerType type, const char* subtype);
    HADeviceTrigger(const char* type, TriggerSubtype subtype);
    HADeviceTrigger(TriggerType type, TriggerSubtype subtype);

    /**
     * Returns type of the trigger.
     * Please note that it may be pointer to the flash memory if you have used enum in the constructor.
     * Use `HADeviceTrigger::isProgmemType` to verify if type points to flash memory.
     *
     * @returns Pointer to the type.
     */
    inline const char* getType() const
        { return _type; }

    /**
     * Returns true if the type pointer points to flash memory.
     *
     * @return Boolean
     */
    inline bool isProgmemType() const
        { return _isProgmemType; }

    /**
     * Returns subtype of the trigger.
     * Please note that it may be pointer to the flash memory if you have used enum in the constructor.
     * Use `HADeviceTrigger::isProgmemSubtype` to verify if type points to flash memory.
     *
     * @returns Pointer to the subtype.
     */
    inline const char* getSubtype() const
        { return _subtype; }

    /**
     * Returns true if the subtype pointer points to flash memory.
     *
     * @return Boolean
     */
    inline bool isProgmemSubtype() const
        { return _isProgmemSubtype; }

    /**
     * Publishes MQTT message with the trigger event.
     *
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool trigger();

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;

private:
    uint16_t calculateIdSize() const;
    void buildUniqueId();
    const char* determineProgmemType(TriggerType type) const;
    const char* determineProgmemSubtype(TriggerSubtype subtype) const;

private:
    const char* _type;
    const char* _subtype;
    bool _isProgmemType;
    bool _isProgmemSubtype;
};

#endif
#endif
