#include <Arduino.h>

#include "DeviceTypeSerializer.h"
#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

static const char CharSlash[] PROGMEM = {"/"};

DeviceTypeSerializer::DeviceTypeSerializer(BaseDeviceType* type) :
    _type(type)
{

}

DeviceTypeSerializer::~DeviceTypeSerializer()
{

}

uint16_t DeviceTypeSerializer::calculateTopicLength(
    const char* component,
    const char* objectId,
    const char* suffix,
    bool includeNullTerminator
) const
{
    const char* prefix = _type->mqtt()->getDiscoveryPrefix();
    if (prefix == nullptr) {
        return 0;
    }

    uint16_t size =
        strlen(prefix) + 1 + // with slash
        strlen(component) + 1 + // with slash
        strlen(suffix); // with null terminator

    if (objectId != nullptr) {
        size += strlen(objectId) + 1; // with slash
    } else {
        size += 1; // slash
    }

    if (_type->mqtt()->getDevice() != nullptr) {
        size += strlen(_type->mqtt()->getDevice()->getUniqueId()) + 1; // with slash
    }

    if (includeNullTerminator) {
        size += 1;
    }

    return size;
}

uint16_t DeviceTypeSerializer::generateTopic(
    char* output,
    const char* component,
    const char* objectId,
    const char* suffix
) const
{
    const char* prefix = _type->mqtt()->getDiscoveryPrefix();
    strcpy(output, prefix);
    strcat_P(output, CharSlash);
    strcat(output, component);
    strcat_P(output, CharSlash);

    if (_type->mqtt()->getDevice() != nullptr) {
        strcat(output, _type->mqtt()->getDevice()->getUniqueId());
        strcat_P(output, CharSlash);
    }

    strcat(output, objectId);
    strcat_P(output, CharSlash);
    strcat(output, suffix);
    return strlen(output) + 1; // size with null terminator
}

uint16_t DeviceTypeSerializer::calculateBaseJsonDataSize()
{
    return 2; // opening and closing brackets of the JSON data
}

uint16_t DeviceTypeSerializer::calculateNameFieldSize()
{
    // Field format: "name":"[NAME]",
    return strlen(_type->name()) + 10; // 10 - length of the JSON decorators for this field
}

uint16_t DeviceTypeSerializer::calculateUniqueIdFieldSize()
{
    // Field format: "uniq_id":"[DEVICE ID]_[NAME]",
    return (
        strlen(_type->mqtt()->getDevice()->getUniqueId()) +
        strlen(_type->name()) +
        14 // 14 - length of the JSON decorators for this field
    );
}

uint16_t DeviceTypeSerializer::calculateAvailabilityFieldSize()
{
    if (!_type->isAvailabilityConfigured()) {
        return 0;
    }

    const uint16_t& availabilityTopicLength = calculateTopicLength(
        _type->componentName(),
        _type->name(),
        BaseDeviceType::AvailabilityTopic,
        false
    );

    if (availabilityTopicLength == 0) {
        return 0;
    }

    // Field format: "avty_t":"[TOPIC]",
    return availabilityTopicLength + 12; // 12 - length of the JSON decorators for this field
}

uint16_t DeviceTypeSerializer::calculateDeviceFieldSize(const char* serializedDevice)
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    // Field format: "dev":[DEVICE],
    return strlen(serializedDevice) + 7; // 7 - length of the JSON decorators for this field
}
