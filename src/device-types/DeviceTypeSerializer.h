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
    virtual uint16_t calculateTopicLength(
        const char* component,
        const char* objectId,
        const char* suffix,
        bool includeNullTerminator = true
    ) const final;

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
    virtual uint16_t generateTopic(
        char* output,
        const char* component,
        const char* objectId,
        const char* suffix
    ) const final;

    virtual uint16_t calculateBaseJsonDataSize() final;
    virtual uint16_t calculateNameFieldSize() final;
    virtual uint16_t calculateUniqueIdFieldSize() final;
    virtual uint16_t calculateAvailabilityFieldSize() final;
    virtual uint16_t calculateDeviceFieldSize(const char* serializedDevice) final;

private:
    BaseDeviceType* _type;
};

#endif
