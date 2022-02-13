#ifndef AHA_BASEDEVICETYPE_H
#define AHA_BASEDEVICETYPE_H

#include <stdint.h>

#include "../ArduinoHADefines.h"

class HAMqtt;
class HASerializer;

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

    inline void setName(const char* name)
        { _name = name; }

    inline const char* getName() const
        { return _name; }

    virtual void setAvailability(bool online);

protected:
    HAMqtt* mqtt() const;

    virtual void buildSerializer() { };
    virtual void destroySerializer();

    virtual void onMqttConnected() = 0;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t& length
    );

    virtual void publishConfig();
    virtual void publishAvailability();
    virtual bool compareTopics(const char* topic, const char* expectedTopic);

    const char* const _componentName;
    const char* const _uniqueId;
    HASerializer* _serializer;
    const char* _name;

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
