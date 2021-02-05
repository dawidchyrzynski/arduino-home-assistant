#ifndef AHA_BASEDEVICETYPE_H
#define AHA_BASEDEVICETYPE_H

#include <stdint.h>

class HAMqtt;

class BaseDeviceType
{
public:
    static const char* ConfigTopic;
    static const char* EventTopic;
    static const char* AvailabilityTopic;
    static const char* Online;
    static const char* Offline;

    BaseDeviceType(HAMqtt& mqtt, const char* componentName, const char* name);
    virtual ~BaseDeviceType();

    virtual void setAvailability(bool online);

    virtual inline bool isOnline() const
        { return (_availability == AvailabilityOnline); }

protected:
    inline HAMqtt* mqtt() const
        { return &_mqtt; }

    inline bool isAvailabilityConfigured() const
        { return (_availability != AvailabilityDefault); }

    virtual void onMqttConnected() = 0;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) { };

    virtual void publishAvailability();

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

    const char* const _componentName;
    const char* const _name;

private:
    enum Availability {
        AvailabilityDefault = 0,
        AvailabilityOnline,
        AvailabilityOffline
    };

    HAMqtt& _mqtt;
    Availability _availability;

    friend class HAMqtt;
};

#endif
