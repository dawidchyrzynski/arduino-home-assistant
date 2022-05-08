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

#ifdef ARDUINOHA_TEST
    inline HASerializer* getSerializer() const
        { return _serializer; }

    inline void publishConfigTest()
        { publishConfig(); }

    inline void publishOnDataTopicTest(
        const char* topicP,
        const char* value,
        bool retained = false,
        bool isProgmemValue = false
    ) {
        publishOnDataTopic(topicP, value, retained, isProgmemValue);
    }
#endif

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
    virtual bool publishOnDataTopic(
        const char* topicP,
        const char* value,
        bool retained = false,
        bool isProgmemValue = false
    );

    const char* const _componentName;
    const char* const _uniqueId;
    const char* _name;
    HASerializer* _serializer;

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
