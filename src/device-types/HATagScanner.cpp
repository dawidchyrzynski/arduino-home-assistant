#include "HATagScanner.h"
#ifdef ARDUINOHA_TAG_SCANNER

#include "../ArduinoHADefines.h"
#include "../HAMqtt.h"
#include "../HADevice.h"

HATagScanner::HATagScanner(const char* name) :
    BaseDeviceType("tag", name)
{

}

HATagScanner::HATagScanner(const char* name, HAMqtt& mqtt) :
    HATagScanner(name)
{
    (void)mqtt;
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
    if (tag == nullptr || strlen(tag) == 0 || strlen(name()) == 0) {
        return false;
    }

    const uint16_t& topicSize = DeviceTypeSerializer::calculateTopicLength(
        componentName(),
        name(),
        DeviceTypeSerializer::EventTopic
    );
    if (topicSize == 0) {
        return false;
    }

    char topic[topicSize];
    DeviceTypeSerializer::generateTopic(
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

    DeviceTypeSerializer::mqttWriteBeginningJson();

    // topic
    {
        static const char Prefix[] PROGMEM = {"\"t\":\""};
        DeviceTypeSerializer::mqttWriteTopicField(
            this,
            Prefix,
            DeviceTypeSerializer::EventTopic
        );
    }

    DeviceTypeSerializer::mqttWriteDeviceField(serializedDevice);
    DeviceTypeSerializer::mqttWriteEndJson();

    return true;
}

#endif
