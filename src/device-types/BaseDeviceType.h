#ifndef BASEDEVICETYPE_H
#define BASEDEVICETYPE_H

#include <stdint.h>

class HAMqtt;

class BaseDeviceType
{
public:
    static const char* ConfigTopic;
    static const char* EventTopic;

    BaseDeviceType(HAMqtt& mqtt);
    virtual ~BaseDeviceType();

    virtual void onMqttConnected() = 0;
    virtual void onMqttMessage(
        const char* topic,
        const char* payload,
        const uint16_t& length
    ) { };

protected:
    inline HAMqtt* mqtt() const
        { return &_mqtt; }

    virtual uint16_t calculateTopicLength(
        const char* component,
        const char* objectId,
        const char* suffix,
        bool includeNullTerminator = true
    ) const final;

    virtual uint16_t generateTopic(
        char* output,
        const char* component,
        const char* objectId,
        const char* suffix
    ) const final;

private:
    HAMqtt& _mqtt;
};

#endif
