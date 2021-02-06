#include "HATagScanner.h"
#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"
#include "../HAUtils.h"

HATagScanner::HATagScanner(const char* name, HAMqtt& mqtt) :
    BaseDeviceType(mqtt, "tag", name)
{

}

void HATagScanner::onMqttConnected()
{
    if (strlen(name()) == 0) {
        return;
    }

    publishConfig();
}

bool HATagScanner::tagScanned(const char* tag)
{
    if (tag == nullptr || strlen(tag) == 0) {
        return false;
    }

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        mqtt(),
        componentName(),
        name(),
        DeviceTypeSerializer::EventTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
        mqtt(),
        topic,
        componentName(),
        name(),
        DeviceTypeSerializer::EventTopic
    );

    if (strlen(topic) == 0) {
        return false;
    }

    return mqtt()->publish(topic, tag);
}

void HATagScanner::publishConfig()
{
    const HADevice* device = mqtt()->getDevice();
    if (device == nullptr) {
        return;
    }

    const uint16_t& deviceLength = device->calculateSerializedLength();
    if (deviceLength == 0) {
        return;
    }

    char serializedDevice[deviceLength];
    if (device->serialize(serializedDevice) == 0) {
        return;
    }

    const uint16_t& topicLength = DeviceTypeSerializer::calculateTopicLength(
        mqtt(),
        componentName(),
        name(),
        DeviceTypeSerializer::ConfigTopic
    );
    const uint16_t& dataLength = calculateSerializedLength(serializedDevice);

    if (topicLength == 0 || dataLength == 0) {
        return;
    }

    char topic[topicLength];
    DeviceTypeSerializer::generateTopic(
        mqtt(),
        topic,
        componentName(),
        name(),
        DeviceTypeSerializer::ConfigTopic
    );

    if (strlen(topic) == 0) {
        return;
    }

    if (mqtt()->beginPublish(topic, dataLength, true)) {
        writeSerializedData(serializedDevice);
        mqtt()->endPublish();
    }
}

uint16_t HATagScanner::calculateSerializedLength(
    const char* serializedDevice
) const
{
    if (serializedDevice == nullptr) {
        return 0;
    }

    uint16_t size = 0;
    size += DeviceTypeSerializer::calculateBaseJsonDataSize();
    size += DeviceTypeSerializer::calculateDeviceFieldSize(serializedDevice);

    // event topic
    {
        const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
            mqtt(),
            componentName(),
            name(),
            DeviceTypeSerializer::EventTopic,
            false
        );

        // Format: "t":"[TOPIC]"
        size += topicSize + 6; // 6 - length of the JSON decorators for this field
    }

    return size; // exludes null terminator
}

bool HATagScanner::writeSerializedData(const char* serializedDevice) const
{
    if (serializedDevice == nullptr) {
        return false;
    }

    DeviceTypeSerializer::mqttWriteBeginningJson(mqtt());

    // topic
    {
        const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
            mqtt(),
            componentName(),
            name(),
            DeviceTypeSerializer::EventTopic
        );
        char topic[topicSize];
        DeviceTypeSerializer::generateTopic(
            mqtt(),
            topic,
            componentName(),
            name(),
            DeviceTypeSerializer::EventTopic
        );

        static const char Prefix[] PROGMEM = {"\"t\":\""};
        DeviceTypeSerializer::mqttWriteConstCharField(mqtt(), Prefix, topic);
    }

    DeviceTypeSerializer::mqttWriteDeviceField(mqtt(), serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson(mqtt());

    return true;
}
