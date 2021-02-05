#ifndef AHA_DEVICETYPESERIALIZER_H
#define AHA_DEVICETYPESERIALIZER_H

#include <stdint.h>

class BaseDeviceType;

class DeviceTypeSerializer
{
public:
    DeviceTypeSerializer(BaseDeviceType* type);
    virtual ~DeviceTypeSerializer();

protected:
    /**
     * Calculates length of the topic with given parameters.
     * Topic format: [discovery prefix]/[component]/[objectId]/[suffix]
     *
     * @param component
     * @param objectId
     * @param suffix
     * @param includeNullTerminator
     */
    uint16_t calculateTopicLength(
        const char* component,
        const char* objectId,
        const char* suffix,
        bool includeNullTerminator = true
    ) const;

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
    uint16_t generateTopic(
        char* output,
        const char* component,
        const char* objectId,
        const char* suffix
    ) const;

    uint16_t calculateBaseJsonDataSize();
    uint16_t calculateNameFieldSize();
    uint16_t calculateUniqueIdFieldSize();
    uint16_t calculateAvailabilityFieldSize();
    uint16_t calculateDeviceFieldSize(const char* serializedDevice);

    void mqttWriteBeginningJson();
    void mqttWriteEndJson();
    void mqttWriteConstCharField(const char* prefix, const char* value);
    void mqttWriteNameField();
    void mqttWriteUniqueIdField();
    void mqttWriteAvailabilityField();
    void mqttWriteDeviceField(const char* serializedDevice);

private:
    BaseDeviceType* _type;
};

#endif
