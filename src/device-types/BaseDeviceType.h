#ifndef BASEDEVICETYPE_H
#define BASEDEVICETYPE_H

#include <stdint.h>

class HAMqtt;

class BaseDeviceType
{
public:
    static const char* ConfigTopic;
    static const char* EventTopic;

    BaseDeviceType(const char* uniqueId, HAMqtt& mqtt);
    virtual ~BaseDeviceType();

    virtual void onMqttConnected() = 0;

    // to do: onMqttMessage

protected:
    inline HAMqtt* mqtt() const
        { return &_mqtt; }

    virtual uint16_t calculateTopicLength(
        const char* haNamespace,
        const char* type,
        const char* subtype,
        const char* suffix,
        bool includeNullTerminator = true
    ) const final;

    virtual uint16_t generateTopic(
        char* output,
        const char* haNamespace,
        const char* type,
        const char* subtype,
        const char* suffix
    ) const final;

private:
    const char* _uniqueId;
    HAMqtt& _mqtt;
};

#endif
