#ifndef AHA_BASEDEVICETYPE_H
#define AHA_BASEDEVICETYPE_H

#include <stdint.h>

class HAMqtt;

class BaseDeviceType
{
public:
    static const char* ConfigTopic;
    static const char* EventTopic;

    BaseDeviceType(HAMqtt& mqtt);
    virtual ~BaseDeviceType();

protected:
    inline HAMqtt* mqtt() const
        { return &_mqtt; }

    virtual void onMqttConnected() = 0;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) { };

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
    enum Availability {
        AvailabilityDefault = 0,
        AvailabilityAvailable,
        AvailabilityUnavailable
    };

    HAMqtt& _mqtt;
    Availability _availability;

    friend class HAMqtt;
};

#endif
