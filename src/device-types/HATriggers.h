#ifndef AHA_HATRIGGERS_H
#define AHA_HATRIGGERS_H

#include "BaseDeviceType.h"

#ifdef ARDUINOHA_TRIGGERS

struct HATrigger {
    const char* type;
    const char* subtype;
} __attribute__((packed));

class HATriggers : public BaseDeviceType
{
public:
    HATriggers();
    HATriggers(HAMqtt& mqtt); // legacy constructor
    virtual ~HATriggers();

    virtual void onMqttConnected() override;

    /**
     * Triggers dont't support availability. Nothing to do here.
     */
    virtual void setAvailability(bool online) override { (void)online; }

    bool add(const char* type, const char* subtype);
    bool trigger(const char* type, const char* subtype);

protected:
    void publishConfig() override;

    uint16_t calculateSerializedLength(
        const char* serializedDevice
    ) const override { (void)serializedDevice; return 0; }

    bool writeSerializedData(
        const char* serializedDevice
    ) const override { (void)serializedDevice; return false; }

    uint16_t calculateTopicLength(
        const char* component,
        const HATrigger *trigger,
        const char* suffix,
        bool includeNullTerminator = true,
        bool isDiscoveryTopic = false
    ) const;

    uint16_t generateTopic(
        char* output,
        const char* component,
        const HATrigger *trigger,
        const char* suffix,
        bool isDiscoveryTopic = false
    ) const;

private:
    uint16_t calculateSerializedLength(
        const HATrigger* trigger,
        const char* serializedDevice
    ) const;

    bool writeSerializedTrigger(
        const HATrigger* trigger,
        const char* serializedDevice
    ) const;

    HATrigger* _triggers;
    uint8_t _triggersNb;
};

#endif
#endif
