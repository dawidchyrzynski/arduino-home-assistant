#include <Arduino.h>

#include "DeviceTypeSerializer.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

static const char CharSlash[] PROGMEM = {"/"};
static const char CharUnderscore[] PROGMEM = {"_"};
static const char CharQuotation[] PROGMEM = {"\""};

const char* DeviceTypeSerializer::ConfigTopic = "config";
const char* DeviceTypeSerializer::EventTopic = "event";
const char* DeviceTypeSerializer::AvailabilityTopic = "avail";
const char* DeviceTypeSerializer::StateTopic = "state";
const char* DeviceTypeSerializer::CommandTopic = "cmd";
const char* DeviceTypeSerializer::Online = "online";
const char* DeviceTypeSerializer::Offline = "offline";
const char* DeviceTypeSerializer::StateOn = "ON";
const char* DeviceTypeSerializer::StateOff = "OFF";

uint16_t DeviceTypeSerializer::calculateTopicLength(
    const HAMqtt* mqtt,
    const char* component,
    const char* objectId,
    const char* suffix,
    bool includeNullTerminator
)
{
    const char* prefix = mqtt->getDiscoveryPrefix();
    if (prefix == nullptr) {
        return 0;
    }

    uint16_t size =
        strlen(prefix) + 1 + // with slash
        strlen(component) + 1 + // with slash
        strlen(suffix);

    if (objectId != nullptr) {
        size += strlen(objectId) + 1; // with slash
    } else {
        size += 1; // slash
    }

    if (mqtt->getDevice() != nullptr) {
        size += strlen(mqtt->getDevice()->getUniqueId()) + 1; // with slash
    }

    if (includeNullTerminator) {
        size += 1;
    }

    return size;
}

uint16_t DeviceTypeSerializer::generateTopic(
    const HAMqtt* mqtt,
    char* output,
    const char* component,
    const char* objectId,
    const char* suffix
)
{
    const char* prefix = mqtt->getDiscoveryPrefix();
    strcpy(output, prefix);
    strcat_P(output, CharSlash);
    strcat(output, component);
    strcat_P(output, CharSlash);

    if (mqtt->getDevice() != nullptr) {
        strcat(output, mqtt->getDevice()->getUniqueId());
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

uint16_t DeviceTypeSerializer::calculateNameFieldSize(const char* name)
{
    if (name == nullptr) {
        return 0;
    }

    // Field format: ,"name":"[NAME]"
    return strlen(name) + 10; // 10 - length of the JSON decorators for this field
}

uint16_t DeviceTypeSerializer::calculateUniqueIdFieldSize(
    const HADevice* device,
    const char* name
)
{
    if (device == nullptr || name == nullptr) {
        return 0;
    }

    // Field format: ,"uniq_id":"[DEVICE ID]_[NAME]"
    return (
        strlen(device->getUniqueId()) +
        strlen(name) +
        14 // 14 - length of the JSON decorators for this field
    );
}

uint16_t DeviceTypeSerializer::calculateAvailabilityFieldSize(
    const HAMqtt* mqtt,
    const char* componentName,
    const char* name
)
{
    if (mqtt == nullptr || componentName == nullptr || name == nullptr) {
        return 0;
    }

    const uint16_t& availabilityTopicLength = calculateTopicLength(
        mqtt,
        componentName,
        name,
        AvailabilityTopic,
        false
    );

    if (availabilityTopicLength == 0) {
        return 0;
    }

    // Field format: ,"avty_t":"[TOPIC]"
    return availabilityTopicLength + 12; // 12 - length of the JSON decorators for this field
}

uint16_t DeviceTypeSerializer::calculateDeviceFieldSize(
    const char* serializedDevice
)
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    // Field format: ,"dev":[DEVICE]
    return strlen(serializedDevice) + 7; // 7 - length of the JSON decorators for this field
}

void DeviceTypeSerializer::mqttWriteBeginningJson(HAMqtt* mqtt)
{
    if (mqtt == nullptr) {
        return;
    }

    static const char Data[] PROGMEM = {"{"};
    mqtt->writePayload_P(Data);
}

void DeviceTypeSerializer::mqttWriteEndJson(HAMqtt* mqtt)
{
    if (mqtt == nullptr) {
        return;
    }

    static const char Data[] PROGMEM = {"}"};
    mqtt->writePayload_P(Data);
}

void DeviceTypeSerializer::mqttWriteConstCharField(
    HAMqtt* mqtt,
    const char* prefix,
    const char* value
)
{
    if (mqtt == nullptr || prefix == nullptr || value == nullptr) {
        return;
    }

    mqtt->writePayload_P(prefix);
    mqtt->writePayload(value, strlen(value));
    mqtt->writePayload_P(CharQuotation);
}

void DeviceTypeSerializer::mqttWriteNameField(HAMqtt* mqtt, const char* name)
{
    if (mqtt == nullptr || name == nullptr) {
        return;
    }

    static const char Prefix[] PROGMEM = {",\"name\":\""};
    mqttWriteConstCharField(mqtt, Prefix, name);
}

void DeviceTypeSerializer::mqttWriteUniqueIdField(
    HAMqtt* mqtt,
    const char* name
)
{
    if (mqtt == nullptr || name == nullptr) {
        return;
    }

    static const char Prefix[] PROGMEM = {",\"uniq_id\":\""};

    uint8_t uniqueIdLength = strlen(name) + strlen(mqtt->getDevice()->getUniqueId()) + 2; // underscore and null temrinator
    char uniqueId[uniqueIdLength];
    strcpy(uniqueId, name);
    strcat_P(uniqueId, CharUnderscore);
    strcat(uniqueId, mqtt->getDevice()->getUniqueId());

    mqttWriteConstCharField(mqtt, Prefix, uniqueId);
}

void DeviceTypeSerializer::mqttWriteAvailabilityField(
    HAMqtt* mqtt,
    const char* componentName,
    const char* name
)
{
    if (mqtt == nullptr || componentName == nullptr || name == nullptr) {
        return;
    }

    const uint16_t& topicSize = calculateTopicLength(
        mqtt,
        componentName,
        name,
        AvailabilityTopic
    );
    if (topicSize == 0) {
        return;
    }

    char availabilityTopic[topicSize];
    generateTopic(
        mqtt,
        availabilityTopic,
        componentName,
        name,
        AvailabilityTopic
    );

    if (strlen(availabilityTopic) == 0) {
        return;
    }

    static const char Prefix[] PROGMEM = {",\"avty_t\":\""};
    mqttWriteConstCharField(mqtt, Prefix, availabilityTopic);
}

void DeviceTypeSerializer::mqttWriteDeviceField(
    HAMqtt* mqtt,
    const char* serializedDevice
)
{
    if (mqtt == nullptr || serializedDevice == nullptr) {
        return;
    }

    static const char Data[] PROGMEM = {",\"dev\":"};

    mqtt->writePayload_P(Data);
    mqtt->writePayload(serializedDevice, strlen(serializedDevice));
}

bool DeviceTypeSerializer::mqttWriteTopicField(
    const char* componentName,
    const char* name,
    const char* jsonPrefix,
    const char* topicSuffix
)
{
    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        HAMqtt::instance(),
        componentName,
        name,
        topicSuffix
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
        HAMqtt::instance(),
        topic,
        componentName,
        name,
        topicSuffix
    );

    if (strlen(topic) == 0) {
        return false;
    }

    DeviceTypeSerializer::mqttWriteConstCharField(HAMqtt::instance(), jsonPrefix, topic);
}

bool DeviceTypeSerializer::mqttPublishMessage(
    const char* componentName,
    const char* name,
    const char* topic,
    const char* data
)
{
    if (componentName == nullptr || name == nullptr ||
            topic == nullptr || data == nullptr) {
        return false;
    }

    const uint16_t& topicSize = calculateTopicLength(
        HAMqtt::instance(),
        componentName,
        name,
        topic
    );
    if (topicSize == 0) {
        return false;
    }

    char finalTopic[topicSize];
    generateTopic(
        HAMqtt::instance(),
        finalTopic,
        componentName,
        name,
        finalTopic
    );

    if (strlen(finalTopic) == 0) {
        return false;
    }

    return HAMqtt::instance()->publish(
        finalTopic,
        data,
        true
    );
}
