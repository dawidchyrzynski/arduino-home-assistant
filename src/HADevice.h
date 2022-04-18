#ifndef AHA_HADEVICE_H
#define AHA_HADEVICE_H

#include <Arduino.h>

class HASerializer;

class HADevice
{
public:
    HADevice();
    HADevice(const char* uniqueId);
    HADevice(const byte* uniqueId, const uint16_t length);
    virtual ~HADevice();

    inline const char* getUniqueId() const
        { return _uniqueId; }

    inline const HASerializer* const getSerializer() const
        { return _serializer; }

    inline bool isSharedAvailabilityEnabled() const
        { return _sharedAvailability; }

    inline const char* getAvailabilityTopic() const
        { return _availabilityTopic; }

    inline bool isAvailable() const
        { return _available; }

    void setManufacturer(const char* manufacturer);
    void setModel(const char* model);
    void setName(const char* name);
    void setSoftwareVersion(const char* softwareVersion);
    void setAvailability(bool online);
    bool enableSharedAvailability();
    bool enableLastWill(bool retained = false);
    bool setUniqueId(const byte* uniqueId, const uint16_t length);
    void publishAvailability();

private:
    HASerializer* _serializer;
    const char* _uniqueId;
    char* _availabilityTopic;
    bool _sharedAvailability;
    bool _available;
};

#endif
