#ifndef AHA_BASEDEVICETYPE_H
#define AHA_BASEDEVICETYPE_H

#include <stdint.h>

#include "DeviceTypeSerializer.h"

class HAMqtt;

class BaseDeviceType : protected DeviceTypeSerializer
{
public:
    static const char* ConfigTopic;
    static const char* EventTopic;
    static const char* AvailabilityTopic;
    static const char* Online;
    static const char* Offline;

    BaseDeviceType(HAMqtt& mqtt, const char* componentName, const char* name);
    virtual ~BaseDeviceType();

    inline bool isOnline() const
        { return (_availability == AvailabilityOnline); }

    virtual void setAvailability(bool online);

protected:
    inline HAMqtt* mqtt() const
        { return &_mqtt; }

    inline const char* name() const
        { return _name; }

    inline const char* componentName() const
        { return _componentName; }

    inline bool isAvailabilityConfigured() const
        { return (_availability != AvailabilityDefault); }

    virtual void onMqttConnected() = 0;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    ) { };

    virtual void publishAvailability();

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
    friend class DeviceTypeSerializer;
};

#endif
