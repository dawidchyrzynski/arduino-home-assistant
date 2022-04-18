
#include <Arduino.h>

#include "HASerializer.h"
#include "../ArduinoHADefines.h"
#include "../HADevice.h"
#include "../HAMqtt.h"
#include "../device-types/BaseDeviceType.h"

uint16_t HASerializer::calculateConfigTopicLength(
    const char* componentName,
    const char* objectId,
    bool includeNullTerminator
)
{
    const HAMqtt* mqtt = HAMqtt::instance();
    if (
        !componentName ||
        !objectId ||
        !mqtt ||
        !mqtt->getDiscoveryPrefix() ||
        !mqtt->getDevice()
    ) {
        return 0;
    }

    uint16_t size =
        strlen(mqtt->getDiscoveryPrefix()) + 1 + // prefix with slash
        strlen(componentName) + 1 + // component name with slash
        strlen(mqtt->getDevice()->getUniqueId()) + 1 + // device ID with slash
        strlen(objectId) + 1 + // object ID with slash
        strlen_P(HAConfigTopic);

    return includeNullTerminator ? size + 1 : size;
}

uint16_t HASerializer::generateConfigTopic(
    char* output,
    const char* componentName,
    const char* objectId
)
{
    const HAMqtt* mqtt = HAMqtt::instance();
    if (
        !output ||
        !componentName ||
        !objectId ||
        !mqtt ||
        !mqtt->getDiscoveryPrefix() ||
        !mqtt->getDevice()
    ) {
        return 0;
    }

    strcpy(output, mqtt->getDiscoveryPrefix());
    strcat_P(output, HASerializerSlash);

    strcat(output, componentName);
    strcat_P(output, HASerializerSlash);

    strcat(output, mqtt->getDevice()->getUniqueId());
    strcat_P(output, HASerializerSlash);

    strcat(output, objectId);
    strcat_P(output, HASerializerSlash);

    strcat_P(output, HAConfigTopic);

    return strlen(output) + 1; // size with null terminator
}

uint16_t HASerializer::calculateDataTopicLength(
    const char* objectId,
    const char* topicP,
    bool includeNullTerminator
)
{
    const HAMqtt* mqtt = HAMqtt::instance();
    if (
        !topicP ||
        !mqtt ||
        !mqtt->getDataPrefix() ||
        !mqtt->getDevice()
    ) {
        return 0;
    }

    uint16_t size =
        strlen(mqtt->getDataPrefix()) + 1 + // prefix with slash
        strlen(mqtt->getDevice()->getUniqueId()) + 1 + // device ID with slash
        strlen_P(topicP);

    if (objectId) {
        size += strlen(objectId) + 1; // object ID with slash;
    }

    return includeNullTerminator ? size + 1 : size;
}

uint16_t HASerializer::generateDataTopic(
    char* output,
    const char* objectId,
    const char* topicP
)
{
    const HAMqtt* mqtt = HAMqtt::instance();
    if (
        !output ||
        !topicP ||
        !mqtt ||
        !mqtt->getDataPrefix() ||
        !mqtt->getDevice()
    ) {
        return 0;
    }

    strcpy(output, mqtt->getDataPrefix());
    strcat_P(output, HASerializerSlash);

    strcat(output, mqtt->getDevice()->getUniqueId());
    strcat_P(output, HASerializerSlash);

    if (objectId) {
        strcat(output, objectId);
        strcat_P(output, HASerializerSlash);
    }

    strcat_P(output, topicP);

    return strlen(output) + 1; // size with null terminator
}

HASerializer::HASerializer(BaseDeviceType* deviceType) :
    _deviceType(deviceType),
    _entries(nullptr),
    _entriesNb(0)
{

}

HASerializer::~HASerializer()
{
    if (_entries) {
        free(_entries);
    }
}

void HASerializer::set(
    const char* propertyP,
    const void* value,
    PropertyValueType valueType
)
{
    if (!propertyP || !value) {
        return;
    }

    SerializerEntry* entry = getExistingEntry(propertyP);
    if (!entry) {
        entry =  addEntry();
    }

    if (!entry) {
        return;
    }

    entry->type = PropertyEntryType;
    entry->subtype = static_cast<uint8_t>(valueType);
    entry->property = propertyP;
    entry->value = value;
}

void HASerializer::set(const FlagType flag)
{
    if (flag == WithDevice) {
        SerializerEntry* entry = addEntry();
        if (!entry) {
            return;
        }

        entry->type = FlagEntryType;
        entry->subtype = static_cast<uint8_t>(InternalWithDevice);
        entry->property = nullptr;
        entry->value = nullptr;
    } else if (flag == WithAvailability) {
        uint8_t subtype = 0;
        HAMqtt* mqtt = HAMqtt::instance();
        const bool isSharedAvailability = mqtt->getDevice()->isSharedAvailabilityEnabled();
        const bool isAvailabilityConfigured = _deviceType->isAvailabilityConfigured();

        if (!isSharedAvailability && !isAvailabilityConfigured) {
            return; // not configured
        }

        SerializerEntry* entry = addEntry();
        if (!entry) {
            return;
        }

        entry->type = TopicEntryType;
        entry->subtype = subtype;
        entry->property = HAAvailabilityTopic;
        entry->value = isSharedAvailability
            ? mqtt->getDevice()->getAvailabilityTopic()
            : nullptr;
    }
}

void HASerializer::topic(const char* topicP)
{
    if (!_deviceType || !topicP) {
        return;
    }

    SerializerEntry* entry = addEntry();
    if (!entry) {
        return;
    }

    entry->type = TopicEntryType;
    entry->subtype = 0;
    entry->property = topicP;
    entry->value = nullptr;
}

HASerializer::SerializerEntry* HASerializer::addEntry()
{
    _entriesNb++;
    _entries = static_cast<SerializerEntry*>(
        realloc(_entries, sizeof(SerializerEntry) * _entriesNb)
    );

    return &_entries[_entriesNb - 1];
}

HASerializer::SerializerEntry* HASerializer::getExistingEntry(
    const char* propertyP
) const
{
    for (uint8_t i = 0; i < _entriesNb; i++) {
        SerializerEntry* entry = &_entries[i];
        if (entry->type == PropertyEntryType && entry->property == propertyP) {
            return entry;
        }
    }

    return nullptr;
}

uint16_t HASerializer::calculateSize() const
{
    HAMqtt* mqtt = HAMqtt::instance();
    if (!mqtt || (_deviceType && !mqtt->getDevice())) {
        return 0;
    }

    uint16_t size =
        strlen_P(HASerializerJsonDataPrefix) +
        strlen_P(HASerializerJsonDataSuffix);
    
    const uint8_t lastEntryIndex = _entriesNb - 1;
    for (uint8_t i = 0; i < _entriesNb; i++) {
        size += calculateEntrySize(&_entries[i], i == lastEntryIndex);
    }

    return size;
}

bool HASerializer::flush() const
{
    HAMqtt* mqtt = HAMqtt::instance();
    if (!mqtt || (_deviceType && !mqtt->getDevice())) {
        return false;
    }

    mqtt->writePayload_P(HASerializerJsonDataPrefix);

    const uint8_t lastEntryIndex = _entriesNb - 1;
    for (uint8_t i = 0; i < _entriesNb; i++) {
        if (!flushEntry(&_entries[i], i == lastEntryIndex)) {
            return false;
        }
    }

    mqtt->writePayload_P(HASerializerJsonDataSuffix);
    return true;
}

uint16_t HASerializer::calculateEntrySize(
    const SerializerEntry* entry,
    bool lastEntry
) const
{
    if (entry->type == UnknownEntryType) {
        return 0;
    }

    uint16_t size = 0;

    if (entry->type == PropertyEntryType && entry->property && entry->value) {
        // property name
        size +=
            strlen_P(HASerializerJsonPropertyPrefix) +
            strlen_P(entry->property) +
            strlen_P(HASerializerJsonPropertySuffix);

        // property value
        size += calculatePropertyValueSize(entry);
    } else if (entry->type == TopicEntryType && entry->property) {
        // property name
        size +=
            strlen_P(HASerializerJsonPropertyPrefix) +
            strlen_P(entry->property) +
            strlen_P(HASerializerJsonPropertySuffix);

        // topic escape
        size += 2 * strlen_P(HASerializerJsonEscapeChar);

        // topic
        if (entry->value) {
            size += strlen(static_cast<const char*>(entry->value));
        } else {
            size += calculateDataTopicLength(
                _deviceType->uniqueId(),
                entry->property,
                false
            );
        }
    } else if (entry->type == FlagEntryType) {
        size += calculateFlagSize(static_cast<FlagInternalType>(entry->subtype));
    }

    if (!lastEntry && size > 0) {
        size += strlen_P(HASerializerJsonPropertiesSeparator);
    }

    return size;
}

uint16_t HASerializer::calculateFlagSize(const FlagInternalType flag) const
{
    const HAMqtt* mqtt = HAMqtt::instance();
    const HADevice* device = mqtt->getDevice();

    if (flag == InternalWithDevice && device->getSerializer()) {
        const uint16_t deviceLength = device->getSerializer()->calculateSize();
        if (deviceLength == 0) {
            return 0;
        }

        return
            strlen_P(HASerializerJsonPropertyPrefix) +
            strlen_P(HADeviceProperty) +
            strlen_P(HASerializerJsonPropertySuffix) +
            deviceLength;
    }

    return 0;
}

uint16_t HASerializer::calculatePropertyValueSize(const SerializerEntry* entry) const
{
    if (entry->type != PropertyEntryType) {
        return 0;
    }

    if (entry->subtype == ConstCharPropertyValue) {
        const char* value = static_cast<const char*>(entry->value);
        if (value) {
            return 2 * strlen_P(HASerializerJsonEscapeChar) + strlen(value);
        }
    } else if (entry->subtype == BoolPropertyType) {
        bool value = *static_cast<const bool*>(entry->value);
        return value ? 4 : 5; // true / false
    }

    // to do: add more types here

    return 0;
}

bool HASerializer::flushEntry(const SerializerEntry* entry, bool lastEntry) const
{
    if (entry->type == UnknownEntryType) {
        return false;
    }

    HAMqtt* mqtt = HAMqtt::instance();

    if (entry->type == PropertyEntryType && entry->property && entry->value) {
        mqtt->writePayload_P(HASerializerJsonPropertyPrefix);
        mqtt->writePayload_P(entry->property);
        mqtt->writePayload_P(HASerializerJsonPropertySuffix);
        
        if (!flushEntryValue(entry)) {
            return false;
        }
    } else if (entry->type == TopicEntryType) {
        if (!flushTopic(entry)) {
            return false;
        }
    } else if (entry->type == FlagEntryType) {
        if (!flushFlag(entry)) {
            return true; // ignore flag
        }
    }

    if (!lastEntry) {
        mqtt->writePayload_P(HASerializerJsonPropertiesSeparator);
    }

    return true;
}

bool HASerializer::flushEntryValue(const SerializerEntry* entry) const
{
    HAMqtt* mqtt = HAMqtt::instance();

    if (entry->subtype == ConstCharPropertyValue) {
        const char* value = static_cast<const char*>(entry->value);
        if (!value) {
            return false;
        }

        mqtt->writePayload_P(HASerializerJsonEscapeChar);
        mqtt->writePayload(value, strlen(value));
        mqtt->writePayload_P(HASerializerJsonEscapeChar);

        return true;
    } else if (entry->subtype == BoolPropertyType) {
        bool value = *static_cast<const bool*>(entry->value);

        mqtt->writePayload_P(value ? HATrue : HAFalse);
        return true;
    }

    // to do: add more types here

    return false;;
}

bool HASerializer::flushTopic(const SerializerEntry* entry) const
{
    HAMqtt* mqtt = HAMqtt::instance();

    // property name
    mqtt->writePayload_P(HASerializerJsonPropertyPrefix);
    mqtt->writePayload_P(entry->property);
    mqtt->writePayload_P(HASerializerJsonPropertySuffix);

    // value (escaped)
    mqtt->writePayload_P(HASerializerJsonEscapeChar);
    
    if (entry->value) {
        const char* topic = static_cast<const char*>(entry->value);
        mqtt->writePayload(topic, strlen(topic));
    } else {
        const uint16_t length = calculateDataTopicLength(
            _deviceType->uniqueId(),
            entry->property
        );

        if (length == 0) {
            return false;
        }

        char topic[length];
        generateDataTopic(
            topic,
            _deviceType->uniqueId(),
            entry->property
        );

        mqtt->writePayload(topic, strlen(topic));
    }

    mqtt->writePayload_P(HASerializerJsonEscapeChar);

    return true;
}

bool HASerializer::flushFlag(const SerializerEntry* entry) const
{
    HAMqtt* mqtt = HAMqtt::instance();
    const HADevice* device = mqtt->getDevice();
    const FlagInternalType& flag = static_cast<FlagInternalType>(entry->subtype);

    if (flag == InternalWithDevice && device->getSerializer()) {
        mqtt->writePayload_P(HASerializerJsonPropertyPrefix);
        mqtt->writePayload_P(HADeviceProperty);
        mqtt->writePayload_P(HASerializerJsonPropertySuffix);

        return device->getSerializer()->flush();
    }

    return false;
}