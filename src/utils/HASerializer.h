
#ifndef AHA_SERIALIZER_H
#define AHA_SERIALIZER_H

#include <stdint.h>

#include "HADictionary.h"
#include "HASerializerArray.h"

class HAMqtt;
class HABaseDeviceType;

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
        ConstCharPropertyValue = 1,
        ProgmemPropertyValue,
        BoolPropertyType,
        Int32PropertyType,
        ArrayPropertyType
    };

    struct SerializerEntry {
        EntryType type;
        uint8_t subtype; // FlagInternalType, PropertyValueType or TopicType
        const char* property;
        const void* value;

        SerializerEntry():
            type(UnknownEntryType),
            subtype(0),
            property(nullptr),
            value(nullptr)
        { }
    };

    static uint16_t calculateConfigTopicLength(
        const char* component,
        const char* objectId
    );

    static bool generateConfigTopic(
        char* output,
        const char* component,
        const char* objectId
    );

    static uint16_t calculateDataTopicLength(
        const char* objectId,
        const char* topicP
    );

    static bool generateDataTopic(
        char* output,
        const char* objectId,
        const char* topicP
    );

    static bool compareDataTopics(
        const char* topic,
        const char* objectId,
        const char* topicP
    );

    HASerializer(HABaseDeviceType* deviceType, const uint8_t maxEntriesNb);
    ~HASerializer();

    inline uint8_t getEntriesNb() const
        { return _entriesNb; }

    inline SerializerEntry* getEntries() const
        { return _entries; }

    void set(
        const char* propertyP,
        const void* value,
        PropertyValueType valueType = ConstCharPropertyValue
    );
    void set(const FlagType flag);
    void topic(const char* topicP);

    uint16_t calculateSize() const;
    bool flush() const;

private:
    enum FlagInternalType {
        InternalWithDevice = 1,
        InternalWithSharedAvailability,
        InternalWithSeparateAvailability
    };

    HABaseDeviceType* _deviceType;
    uint8_t _entriesNb;
    uint8_t _maxEntriesNb;
    SerializerEntry* _entries;

    SerializerEntry* addEntry();
    uint16_t calculateEntrySize(const SerializerEntry* entry) const;
    uint16_t calculateTopicEntrySize(const SerializerEntry* entry) const;
    uint16_t calculateFlagSize(const FlagInternalType flag) const;
    uint16_t calculatePropertyValueSize(const SerializerEntry* entry) const;
    uint16_t calculateArraySize(const HASerializerArray* array) const;
    bool flushEntry(const SerializerEntry* entry) const;
    bool flushEntryValue(const SerializerEntry* entry) const;
    bool flushTopic(const SerializerEntry* entry) const;
    bool flushFlag(const SerializerEntry* entry) const;
};

#endif
