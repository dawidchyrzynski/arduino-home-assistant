#include <Arduino.h>

#include "DeviceTypeSerializer.h"
#include "BaseDeviceType.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

static const char CharSlash[] PROGMEM = {"/"};
static const char CharUnderscore[] PROGMEM = {"_"};
static const char CharQuotation[] PROGMEM = {"\""};

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

void DeviceTypeSerializer::mqttWriteBeginningJson()
{
    static const char Data[] PROGMEM = {"{"};
    _type->mqtt()->writePayload_P(Data);
}

void DeviceTypeSerializer::mqttWriteEndJson()
{
    static const char Data[] PROGMEM = {"}"};
    _type->mqtt()->writePayload_P(Data);
}

void DeviceTypeSerializer::mqttWriteConstCharField(const char* prefix, const char* value)
{
    _type->mqtt()->writePayload_P(prefix);
    _type->mqtt()->writePayload(value, strlen(value));
    _type->mqtt()->writePayload_P(CharQuotation);
}

void DeviceTypeSerializer::mqttWriteNameField()
{
    static const char Prefix[] PROGMEM = {",\"name\":\""};
    mqttWriteConstCharField(Prefix, _type->name());
}

void DeviceTypeSerializer::mqttWriteUniqueIdField()
{
    static const char Prefix[] PROGMEM = {",\"uniq_id\":\""};

    const HADevice* device = _type->mqtt()->getDevice();
    if (device == nullptr) {
        return;
    }

    uint8_t uniqueIdLength = strlen(_type->name()) + strlen(device->getUniqueId()) + 2; // underscore and null temrinator
    char uniqueId[uniqueIdLength];
    strcpy(uniqueId, _type->name());
    strcat_P(uniqueId, CharUnderscore);
    strcat(uniqueId, device->getUniqueId());

    mqttWriteConstCharField(Prefix, uniqueId);
}

void DeviceTypeSerializer::mqttWriteAvailabilityField()
{
    if (!_type->isAvailabilityConfigured()) {
        return;
    }

    const uint16_t& topicSize = calculateTopicLength(
        _type->componentName(),
        _type->name(),
        BaseDeviceType::AvailabilityTopic
    );
    if (topicSize == 0) {
        return;
    }

    char availabilityTopic[topicSize];
    generateTopic(
        availabilityTopic,
        _type->componentName(),
        _type->name(),
        BaseDeviceType::AvailabilityTopic
    );

    if (strlen(availabilityTopic) == 0) {
        return;
    }

    static const char Prefix[] PROGMEM = {",\"avty_t\":\""};
    mqttWriteConstCharField(Prefix, availabilityTopic);
}

void DeviceTypeSerializer::mqttWriteDeviceField(const char* serializedDevice)
{
    static const char Data[] PROGMEM = {",\"dev\":"};

    _type->mqtt()->writePayload_P(Data);
    _type->mqtt()->writePayload(serializedDevice, strlen(serializedDevice));
}
