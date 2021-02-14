#ifndef AHA_DEVICETYPESERIALIZER_H
#define AHA_DEVICETYPESERIALIZER_H

#include <stdint.h>

class HADevice;
class BaseDeviceType;

class DeviceTypeSerializer
{
public:
    static const char* ConfigTopic;
    static const char* EventTopic;
    static const char* AvailabilityTopic;
    static const char* StateTopic;
    static const char* CommandTopic;
    static const char* Online;
    static const char* Offline;
    static const char* StateOn;
    static const char* StateOff;

    /**
     * Calculates length of the topic with given parameters.
     * Topic format: [discovery prefix]/[component]/[objectId]/[suffix]
     *
     * @param component
     * @param objectId
     * @param suffix
     * @param includeNullTerminator
     */
    static uint16_t calculateTopicLength(
        const char* component,
        const char* objectId,
        const char* suffix,
        bool includeNullTerminator = true
    );

    /**
     * Generates topic and saves it to the given buffer.
     * Please note that size of the buffer must be calculated by `calculateTopicLength` method first.
     * Topic format: [discovery prefix]/[component]/[objectId]/[suffix]
     *
     * @param output
     * @param component
     * @param objectId
     * @param suffix
     * @param includeNullTerminator
     */
    static uint16_t generateTopic(
        char* output,
        const char* component,
        const char* objectId,
        const char* suffix
    );

    static uint16_t calculateBaseJsonDataSize();
    static uint16_t calculateNameFieldSize(
        const char* name
    );
    static uint16_t calculateUniqueIdFieldSize(
        const HADevice* device,
        const char* name
    );
    static uint16_t calculateAvailabilityFieldSize(
        const char* componentName,
        const char* name
    );
    static uint16_t calculateDeviceFieldSize(
        const char* serializedDevice
    );

    static void mqttWriteBeginningJson();
    static void mqttWriteEndJson();
    static void mqttWriteConstCharField(
        const char* prefix,
        const char* value
    );
    static void mqttWriteNameField(
        const char* name
    );
    static void mqttWriteUniqueIdField(
        const char* name
    );
    static void mqttWriteAvailabilityField(
        const char* componentName,
        const char* name
    );
    static void mqttWriteDeviceField(
        const char* serializedDevice
    );
    static bool mqttWriteTopicField(
        const char* componentName,
        const char* name,
        const char* jsonPrefix,
        const char* topicSuffix
    );
    static bool mqttPublishMessage(
        const char* componentName,
        const char* name,
        const char* topic,
        const char* data
    );
};

#endif
