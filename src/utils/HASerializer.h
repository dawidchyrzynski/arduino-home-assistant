
#ifndef AHA_SERIALIZER_H
#define AHA_SERIALIZER_H

#include <stdint.h>

#include "HADictionary.h"
#include "HASerializerArray.h"

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
        ConstCharPropertyValue = 1,
        BoolPropertyType,
        FloatP1PropertyType, // 1 digit after decimal
        FloatP2PropertyType, // 2 digits after decimal
        Int32PropertyType,
        ArrayPropertyType
    };

    struct SerializerEntry {
        EntryType type;
        uint8_t subtype; // FlagInternalType, PropertyValueType or TopicType
        const char* property;
        const void* value;
    };

    static uint16_t calculateConfigTopicLength(
        const char* component,
        const char* objectId,
        bool includeNullTerminator = true
    );

    static bool generateConfigTopic(
        char* output,
        const char* component,
        const char* objectId
    );

    static uint16_t calculateDataTopicLength(
        const char* objectId,
        const char* topicP,
        bool includeNullTerminator = true
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

    HASerializer(BaseDeviceType* deviceType);
    virtual ~HASerializer();

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

    BaseDeviceType* _deviceType;
    SerializerEntry* _entries;
    uint8_t _entriesNb;

    SerializerEntry* addEntry();
    SerializerEntry* getExistingEntry(const EntryType type, const char* propertyP) const;
    uint16_t calculateEntrySize(const SerializerEntry* entry, bool lastEntry) const;
    uint16_t calculateFlagSize(const FlagInternalType flag) const;
    uint16_t calculatePropertyValueSize(const SerializerEntry* entry) const;
    uint16_t calculateArraySize(const HASerializerArray* array) const;
    bool flushEntry(const SerializerEntry* entry) const;
    bool flushEntryValue(const SerializerEntry* entry) const;
    bool flushTopic(const SerializerEntry* entry) const;
    bool flushFlag(const SerializerEntry* entry) const;
};

#endif
