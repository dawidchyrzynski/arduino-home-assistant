#ifndef AHA_BASEDEVICETYPE_H
#define AHA_BASEDEVICETYPE_H

#include <stdint.h>

#include "../ArduinoHADefines.h"
#include "DeviceTypeSerializer.h"

class HAMqtt;

class BaseDeviceType
{
public:
    BaseDeviceType(
        const char* componentName,
        const char* uniqueId
    );
    virtual ~BaseDeviceType();

    inline const char* uniqueId() const
        { return _uniqueId; }

    inline const char* componentName() const
        { return _componentName; }

    inline bool isAvailabilityConfigured() const
        { return (_availability != AvailabilityDefault); }

    inline bool isOnline() const
        { return (_availability == AvailabilityOnline); }

    virtual void setAvailability(bool online);

protected:
    HAMqtt* mqtt() const;

    virtual void onMqttConnected() = 0;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    );

    virtual void publishConfig();
    virtual void publishAvailability();
    virtual bool isMyTopic(const char* topic, const char* expectedTopic);
    virtual uint16_t calculateSerializedLength(const char* serializedDevice) const = 0;
    virtual bool writeSerializedData(const char* serializedDevice) const = 0;

    const char* const _componentName;
    const char* const _uniqueId;

private:
    enum Availability {
        AvailabilityDefault = 0,
        AvailabilityOnline,
        AvailabilityOffline
    };

    Availability _availability;

    friend class HAMqtt;
};

#endif
