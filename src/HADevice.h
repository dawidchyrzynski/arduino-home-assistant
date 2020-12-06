#ifndef HADEVICE_H
#define HADEVICE_H

#include <Arduino.h>

class HADevice
{
public:
    HADevice(const char* uniqueId);
    HADevice(const byte* uniqueId, const uint16_t& length);

    inline const char* getUniqueId() const
        { return _uniqueId; }

    inline void setManufacturer(const char* manufacturer)
        { _manufacturer = manufacturer; }

    inline void setModel(const char* model)
        { _model = model; }

    inline void setName(const char* name)
        { _name = name; }

    inline void setSoftwareVersion(const char* softwareVersion)
        { _softwareVersion = softwareVersion; }

    uint16_t calculateSerializedLength() const;
    uint16_t serialize(char* dst) const;

private:
    const char* _uniqueId;
    const char* _manufacturer;
    const char* _model;
    const char* _name;
    const char* _softwareVersion;
};

#endif
