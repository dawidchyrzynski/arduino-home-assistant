#ifndef HATRIGGERS_H
#define HATRIGGERS_H

#include <stdint.h>

#include "BaseDeviceType.h"

struct HATrigger {
    const char* type;
    const char* subtype;
} __attribute__((packed));

class HATriggers : public BaseDeviceType
{
public:
    static const char* MqttNamespace;

    HATriggers(HAMqtt& mqtt);
    virtual ~HATriggers();

    virtual void onMqttConnected() override;

    bool add(const char* type, const char* subtype);
    bool trigger(const char* type, const char* subtype);

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
