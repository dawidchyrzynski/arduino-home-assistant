#ifndef AHA_HADEVICETRIGGER_H
#define AHA_HADEVICETRIGGER_H

#include "BaseDeviceType.h"

#ifndef EX_ARDUINOHA_DEVICE_TRIGGER

class HADeviceTrigger : public BaseDeviceType
{
public:
    /**
     * Creates a new trigge based on the given type and subtype.
     * Please note that combination of type and subtype needs to be unique in a scope of the device.
     * 
     * The list of built-in types is available here: https://www.home-assistant.io/integrations/device_trigger.mqtt/#type
     * The list of built-in subtypes is available here: https://www.home-assistant.io/integrations/device_trigger.mqtt/#subtype
     */
    HADeviceTrigger(const char* type, const char* subtype);

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

private:
    const char* _type;
    const char* _subtype;
};

#endif
#endif
