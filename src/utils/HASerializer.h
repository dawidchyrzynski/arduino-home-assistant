
#ifndef AHA_SERIALIZER_H
#define AHA_SERIALIZER_H

#include <stdint.h>

#include "HADictionary.h"

class HAMqtt;
class BaseDeviceType;

class HASerializer
{
public:
    enum EntryType {
        UnknownEntryType = 0,
        PropertyEntryType,
        TopicEntryType,
        FlagEntryType
    };

    enum FlagType {
        WithDevice = 1,
        WithAvailability
    };

    enum PropertyValueType {
        ConstCharPropertyValue = 1
    };

    struct SerializerEntry {
        EntryType type;
        uint8_t subtype; // FlagType, PropertyValueType or TopicType
        const char* property;
        const void* value;
    };

    static uint16_t calculateDataTopicLength(
        const char* objectId,
        const char* topicP,
        bool includeNullTerminator = true
    );

    static uint16_t generateDataTopic(
        char* output,
        const char* objectId,
        const char* topicP
    );

    HASerializer(BaseDeviceType* deviceType);
    virtual ~HASerializer();

    void set(const char* propertyP, const void* value);
    void set(const FlagType flag);
    void topic(const char* topicP);

    uint16_t calculateSize() const;
    void flush();

private:
    enum FlagInternalEnum {
        InternalWithDevice = 1,
        InternalWithSharedAvailability,
        InternalWithSeparateAvailability
    };

    BaseDeviceType* _deviceType;
    SerializerEntry* _entries;
    uint8_t _entriesNb;

    SerializerEntry* addEntry();
    uint16_t calculateEntrySize(const SerializerEntry* entry, bool lastEntry) const;
    uint16_t calculateFlagSize(const FlagInternalEnum flag) const;
    uint16_t calculatePropertyValueSize(const SerializerEntry* entry) const;
    PropertyValueType determinePropertyValueType(const char* propertyP) const;
    bool flushEntry(const SerializerEntry* entry, bool lastEntry);
    bool flushEntryValue(const SerializerEntry* entry);
    bool flushTopic(const SerializerEntry* entry);
    bool flushFlag(const SerializerEntry* entry);
};

#endif
