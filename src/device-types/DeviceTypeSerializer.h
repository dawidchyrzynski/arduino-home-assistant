#ifndef AHA_DEVICETYPESERIALIZER_H
#define AHA_DEVICETYPESERIALIZER_H

#include <stdint.h>

class HAMqtt;
class HADevice;

class DeviceTypeSerializer
{
public:
    static const char* ConfigTopic;
    static const char* EventTopic;
    static const char* AvailabilityTopic;
    static const char* StateTopic;
    static const char* ValueTopic;
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
        const HAMqtt* mqtt,
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
        const HAMqtt* mqtt,
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
        const HAMqtt* mqtt,
        const char* componentName,
        const char* name
    );
    static uint16_t calculateDeviceFieldSize(
        const char* serializedDevice
    );

    static void mqttWriteBeginningJson(HAMqtt* mqtt);
    static void mqttWriteEndJson(HAMqtt* mqtt);
    static void mqttWriteConstCharField(
        HAMqtt* mqtt,
        const char* prefix,
        const char* value
    );
    static void mqttWriteNameField(HAMqtt* mqtt, const char* name);
    static void mqttWriteUniqueIdField(
        HAMqtt* mqtt,
        const char* name
    );
    static void mqttWriteAvailabilityField(
        HAMqtt* mqtt,
        const char* componentName,
        const char* name
    );
    static void mqttWriteDeviceField(
        HAMqtt* mqtt,
        const char* serializedDevice
    );
};

#endif
